/******************************************************************************
 * @file    IndivFunctionalityFragment.java
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * ---------------------------------------------------------------------------
 *
 *******************************************************************************/
package com.samautotester.view;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.design.widget.FloatingActionButton;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.samautotester.R;
import com.samautotester.controller.IndivTask;
import com.samautotester.controller.SAMExecutor;
import com.samautotester.controller.TestFunc;
import com.samautotester.model.State;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import junit.framework.Test;

public class IndivFunctionalityFragment extends Fragment implements
        View.OnClickListener {

    private String LOG_TAG = IndivFunctionalityFragment.class.getName();

    private enum FABS {
        CARD_STATUS, STREAM_APDU, SWITCH_SLOT, CARD_POWER_UP, CARD_POWER_DOWN
    };

    private IndivTask cardPowerDown_task = null;
    private IndivTask cardPowerUp_task = null;
    private IndivTask getCardState_task = null;
    private IndivTask switchslot_task = null;
    private IndivTask streamApdu_task = null;
    private IndivTask getCardATR_task = null;
    private IndivTask slotMapping_task = null;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.indivfunctionality_fragment,
                container, false);
        // Toast.makeText(getContext(),"onCreateView",Toast.LENGTH_SHORT).show();

        (view.findViewById(R.id.fabCardState)).setOnClickListener(this);
        (view.findViewById(R.id.fabCardATR)).setOnClickListener(this);
        (view.findViewById(R.id.fabSlotMapping)).setOnClickListener(this);
        (view.findViewById(R.id.fabStreamApdu)).setOnClickListener(this);
        (view.findViewById(R.id.fabSwitchSlot)).setOnClickListener(this);
        (view.findViewById(R.id.fabCardPowerUp)).setOnClickListener(this);
        (view.findViewById(R.id.fabCardPowerDown)).setOnClickListener(this);

        // refill the previous data
        // Arrays.fill(fabs_buttons_status, false);

        return view;
    }

    @Override
    public void onClick(final View v) {

        switch (v.getId()) {
        case R.id.fabCardPowerDown:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - CARD_POWER_DOWN ");

                // Reading data from UI
                String cardPowerDown_slotID = ((EditText) getActivity()
                        .findViewById(R.id.cardPowerDown_slotID)).getText()
                        .toString();
                TestFunc.INDIVIDUAL
                        .set_CardPowerDown_slotID(cardPowerDown_slotID);
                TestFunc.INDIVIDUAL.set_CardPowerDown_state(State.RUNNING);

                int slot_id;
                try {
                    slot_id = Integer.parseInt(cardPowerDown_slotID);
                } catch (Exception e) {
                    Toast.makeText(getActivity(), " Invalid 'SlotId' ",
                            Toast.LENGTH_SHORT).show();
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    TestFunc.INDIVIDUAL.set_CardPowerDown_state(State.STOPPED);
                    ((TextView) getView().findViewById(
                            R.id.txtCardPowerDownReturnValue)).setText("");
                    ((TextView) getView().findViewById(
                            R.id.txtCardPowerDownResult)).setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarCardPowerDown))
                        .setVisibility(View.VISIBLE);

                // Run the task ...
                cardPowerDown_task = SAMExecutor.INSTANCE.cardPowerDown(
                        TestFunc.INDIVIDUAL, slot_id, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarCardPowerDown)));
                cardPowerDown_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_CardPowerDown_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_CardPowerDown_result().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtCardPowerDownReturnValue))
                                .setText(return_value + "");
                        ((TextView) getView().findViewById(
                                R.id.txtCardPowerDownResult)).setText(result);

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL
                                .set_CardPowerDown_state(State.STOPPED);
                    }
                });
                cardPowerDown_task.execute();

            } else {

                Log.d(LOG_TAG, "STOP - CARD_POWER_DOWN ");

                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarCardPowerDown)).setVisibility(View.INVISIBLE);

                TestFunc.INDIVIDUAL.set_CardPowerDown_state(State.STOPPED);

                // Stop the task ...
                cardPowerDown_task.cancel(true);
            }

            break;

        case R.id.fabCardPowerUp:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - CARD_POWER_UP ");

                // Reading data from UI
                String cardPowerUp_slotID = ((EditText) getActivity()
                        .findViewById(R.id.cardPowerUp_slotID)).getText()
                        .toString();
                TestFunc.INDIVIDUAL.set_CardPowerUp_slotID(cardPowerUp_slotID);
                TestFunc.INDIVIDUAL.set_CardPowerUp_state(State.RUNNING);

                int slot_id;
                try {
                    slot_id = Integer.parseInt(cardPowerUp_slotID);
                } catch (Exception e) {
                    Toast.makeText(getActivity(), " Invalid 'SlotId' ",
                            Toast.LENGTH_SHORT).show();
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    TestFunc.INDIVIDUAL.set_CardPowerUp_state(State.STOPPED);

                    ((TextView) getView().findViewById(
                            R.id.txtCardPowerUpReturnValue)).setText("");
                    ((TextView) getView().findViewById(
                            R.id.txtCardPowerUpResult)).setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarCardPowerUp))
                        .setVisibility(View.VISIBLE);

                // Run the task ...
                cardPowerUp_task = SAMExecutor.INSTANCE.cardPowerUp(
                        TestFunc.INDIVIDUAL, slot_id, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarCardPowerUp)));
                cardPowerUp_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_CardPowerUp_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_CardPowerUp_result().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtCardPowerUpReturnValue))
                                .setText(return_value + "");
                        ((TextView) getView().findViewById(
                                R.id.txtCardPowerUpResult)).setText(result);

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL
                                .set_CardPowerUp_state(State.STOPPED);
                    }
                });
                cardPowerUp_task.execute();
            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarCardPowerUp)).setVisibility(View.INVISIBLE);

                Log.d(LOG_TAG, "STOP - CARD_POWER_UP ");

                TestFunc.INDIVIDUAL.set_CardPowerUp_state(State.STOPPED);

                // Stop the task ...
                cardPowerUp_task.cancel(true);
            }

            break;
        case R.id.fabCardState:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - GET_CARD_STATE ");

                // Reading data from UI
                String cardState_slotID = ((EditText) getActivity()
                        .findViewById(R.id.cardState_slotID)).getText()
                        .toString();
                TestFunc.INDIVIDUAL.set_CardState_slotID(cardState_slotID);
                TestFunc.INDIVIDUAL.set_CardState_state(State.RUNNING);

                int slot_id;
                try {
                    slot_id = Integer.parseInt(cardState_slotID);
                } catch (Exception e) {
                    Toast.makeText(getActivity(), " Invalid 'SlotId' ",
                            Toast.LENGTH_SHORT).show();
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    TestFunc.INDIVIDUAL.set_CardState_state(State.STOPPED);

                    ((TextView) getView().findViewById(
                            R.id.txtGetCardStateReturnValue)).setText("");
                    ((TextView) getView().findViewById(
                            R.id.txtGetCardStateResult)).setText("");
                    ((TextView) getView().findViewById(
                            R.id.txtGetCardStateCallback)).setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarGetCardState))
                        .setVisibility(View.VISIBLE);

                // Reading data from UI
                TestFunc.INDIVIDUAL
                        .set_CardState_slotID(((EditText) getActivity()
                                .findViewById(R.id.cardState_slotID)).getText()
                                .toString());
                TestFunc.INDIVIDUAL.set_CardState_state(State.RUNNING);

                // Run the task ...
                getCardState_task = SAMExecutor.INSTANCE.getCardState(
                        TestFunc.INDIVIDUAL, slot_id, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarGetCardState)));
                getCardState_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_CardState_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_CardState_result().toString();
                        String callbackResult = TestFunc.INDIVIDUAL
                                .get_CardState_callback().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtGetCardStateReturnValue))
                                .setText(return_value + "");
                        ((TextView) getView().findViewById(
                                R.id.txtGetCardStateResult)).setText(result);
                        ((TextView) getView().findViewById(
                                R.id.txtGetCardStateCallback))
                                .setText(callbackResult);
                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL.set_CardState_state(State.STOPPED);
                    }
                });
                getCardState_task.execute();
            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarGetCardStatus)).setVisibility(View.INVISIBLE);

                Log.d(LOG_TAG, "STOP - GET_CARD_STATE ");

                TestFunc.INDIVIDUAL.set_CardState_state(State.STOPPED);

                // Stop the task ...
                getCardState_task.cancel(true);
            }

            break;

        case R.id.fabCardATR:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - GET_CARD_ATR ");

                // Reading data from UI
                String cardATR_slotID = ((EditText) getActivity().findViewById(
                        R.id.getCardATRSlotId)).getText().toString();
                TestFunc.INDIVIDUAL.set_CardATR_slotID(cardATR_slotID);
                TestFunc.INDIVIDUAL.set_CardATR_state(State.RUNNING);

                int slot_id;
                try {
                    slot_id = Integer.parseInt(cardATR_slotID);
                } catch (Exception e) {
                    Toast.makeText(getActivity(), " Invalid 'SlotId' ",
                            Toast.LENGTH_SHORT).show();
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    TestFunc.INDIVIDUAL.set_CardATR_state(State.STOPPED);

                    ((TextView) getView().findViewById(
                            R.id.txtGetCardATRReturnValue)).setText("");
                    ((TextView) getView()
                            .findViewById(R.id.txtGetCardATRResult))
                            .setText("");
                    ((TextView) getView().findViewById(
                            R.id.txtGetCardATRCallback)).setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarGetCardATR))
                        .setVisibility(View.VISIBLE);

                TestFunc.INDIVIDUAL.set_CardATR_slotID(cardATR_slotID);
                TestFunc.INDIVIDUAL.set_CardATR_state(State.RUNNING);

                // Run the task ...
                getCardATR_task = SAMExecutor.INSTANCE.getCardATR(
                        TestFunc.INDIVIDUAL, slot_id, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarGetCardATR)));
                getCardATR_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_CardATR_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_CardATR_result().toString();
                        String callback_result = TestFunc.INDIVIDUAL
                                .get_CardATR_callback().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtGetCardATRReturnValue))
                                .setText(return_value + "");

                        ((TextView) getView().findViewById(
                                R.id.txtGetCardATRResult)).setText(result);
                        ((TextView) getView().findViewById(
                                R.id.txtGetCardATRCallback))
                                .setText(callback_result);

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL.set_CardATR_state(State.STOPPED);
                    }
                });
                getCardATR_task.execute();
            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarGetCardStatus)).setVisibility(View.INVISIBLE);

                Log.d(LOG_TAG, "STOP - GET_CARD_STATE ");

                TestFunc.INDIVIDUAL.set_CardATR_state(State.STOPPED);

                // Stop the task ...
                getCardATR_task.cancel(true);
            }

            break;

        case R.id.fabSlotMapping:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - SLOT_MAPPING ");

                /*
                 * // Reading data from UI String[] slotmap = (((EditText)
                 * getActivity().findViewById(
                 * R.id.slotMap)).getText().toString()).split(" "); int[]
                 * slotmap_list = new int[slotmap.length];
                 * 
                 * try{ for(int i=0;i<slotmap.length;i++){ slotmap_list[i] =
                 * Integer.parseInt(slotmap[i]); } }catch(Exception e){
                 * Toast.makeText(getActivity(), " Invalid 'slotMap' ",
                 * Toast.LENGTH_SHORT).show(); ((FloatingActionButton) v)
                 * .setImageResource(R.drawable.play);
                 * TestFunc.INDIVIDUAL.set_GetSlotMapping_state(State.STOPPED);
                 * 
                 * ((TextView) getView().findViewById(
                 * R.id.txtGetSlotMappingReturnValue)).setText(""); return; }
                 */

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarGetSlotMapping))
                        .setVisibility(View.VISIBLE);

                // TestFunc.INDIVIDUAL.set_GetSlotMapping_slotMap(slotmap_list);

                // Run the task ...
                slotMapping_task = SAMExecutor.INSTANCE.getSlotMapping(
                        TestFunc.INDIVIDUAL, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarGetSlotMapping)));
                slotMapping_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_GetSlotMapping_returnValue();
                        int[] return_array = TestFunc.INDIVIDUAL
                                .get_GetSlotMapping_slotMap();
                        StringBuilder builder = new StringBuilder();
                        if (return_array != null) {
                            for (int i = 0; i < return_array.length; i++) {
                                builder.append(return_array[i] + ",");
                            }
                        } else {
                            builder.append("null");
                        }

                        ((TextView) getView().findViewById(
                                R.id.txtGetSlotMappingReturnValue))
                                .setText(return_value + "".toString());
                        ((TextView) getView().findViewById(
                                R.id.txtGetSlotMappingResult))
                                .setText("slotmap : " + builder.toString());

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL.set_SwitchSlot_state(State.STOPPED);
                    }
                });

                slotMapping_task.execute();

            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarSwitchSlot)).setVisibility(View.INVISIBLE);

                Log.d(LOG_TAG, "STOP - SLOT_MAPPING ");

                TestFunc.INDIVIDUAL.set_GetSlotMapping_state(State.STOPPED);

                // Stop the task ...
                slotMapping_task.cancel(true);
            }

            break;

        case R.id.fabSwitchSlot:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);

                Log.d(LOG_TAG, "RUN - SWITCH_SLOT ");

                // Reading data from UI
                String slot_list = ((EditText) getActivity().findViewById(
                        R.id.cardSwitchSlot_slotList)).getText().toString();

                String[] list = slot_list.split(" ");
                int[] arr = new int[list.length];
                List<Integer> listOfSlots = new ArrayList<Integer>();

                try {
                    for (int i = 0; i < list.length; i++) {
                        arr[i] = Integer.parseInt(list[i]);
                        listOfSlots.add(arr[i]);
                    }
                } catch (Exception e) {
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    Toast.makeText(getActivity(), " Invalid 'SlotList' ",
                            Toast.LENGTH_SHORT).show();
                    TestFunc.INDIVIDUAL.set_CardState_state(State.STOPPED);

                    ((TextView) getView().findViewById(
                            R.id.txtSwitchSlotReturnValue)).setText("");
                    ((TextView) getView()
                            .findViewById(R.id.txtSwitchSlotResult))
                            .setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarSwitchSlot))
                        .setVisibility(View.VISIBLE);

                TestFunc.INDIVIDUAL.set_SwitchSlot_slotList(listOfSlots);
                TestFunc.INDIVIDUAL.set_SwitchSlot_state(State.RUNNING);

                // Run the task ...
                switchslot_task = SAMExecutor.INSTANCE.switchSlot(
                        TestFunc.INDIVIDUAL, arr, ((ProgressBar) getView()
                                .findViewById(R.id.progressBarSwitchSlot)));
                switchslot_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_SwitchSlot_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_SwitchSlot_result().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtSwitchSlotReturnValue))
                                .setText(return_value + "");
                        ((TextView) getView().findViewById(
                                R.id.txtSwitchSlotResult)).setText(result);

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL.set_SwitchSlot_state(State.STOPPED);
                    }
                });
                switchslot_task.execute();

            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarSwitchSlot)).setVisibility(View.INVISIBLE);

                Log.d(LOG_TAG, "STOP - SWITCH_SLOT ");

                TestFunc.INDIVIDUAL.set_SwitchSlot_state(State.STOPPED);

                // Stop the task ...
                switchslot_task.cancel(true);
            }

            break;
        case R.id.fabStreamApdu:

            if (((FloatingActionButton) v).getDrawable().getConstantState() == getActivity()
                    .getDrawable(R.drawable.play).getConstantState()) {
                ((FloatingActionButton) v).setImageResource(R.drawable.pause);
                // ((ProgressBar)getView().findViewById(R.id.progressBarStreamApdu)).setVisibility(View.VISIBLE);

                Log.d(LOG_TAG, "RUN - STREAM_APDU ");

                // Reading data from UI
                String slot_id = ((EditText) getActivity().findViewById(
                        R.id.streamApduSlotId)).getText().toString();
                String class_type = ((EditText) getActivity().findViewById(
                        R.id.streamApduClassType)).getText().toString();
                String instruction = ((EditText) getActivity().findViewById(
                        R.id.streamApduInstruction)).getText().toString();
                String param1 = ((EditText) getActivity().findViewById(
                        R.id.streamApduParam1)).getText().toString();
                String param2 = ((EditText) getActivity().findViewById(
                        R.id.streamApduParam2)).getText().toString();
                String param3 = ((EditText) getActivity().findViewById(
                        R.id.streamApduParam3)).getText().toString();
                String apdu_command = ((EditText) getActivity().findViewById(
                        R.id.streamApduCommand)).getText().toString();

                int int_slot_id, int_class_type, int_instruction, int_param1, int_param2, int_param3;

                try {
                    int_slot_id = Integer.parseInt(slot_id);
                    int_class_type = Integer.parseInt(class_type);
                    int_instruction = Integer.parseInt(instruction);
                    int_param1 = Integer.parseInt(param1);
                    int_param2 = Integer.parseInt(param2);
                    int_param3 = Integer.parseInt(param3);

                } catch (Exception e) {
                    ((FloatingActionButton) v)
                            .setImageResource(R.drawable.play);
                    Toast.makeText(getActivity(), " Invalid 'input' ",
                            Toast.LENGTH_SHORT).show();
                    TestFunc.INDIVIDUAL.set_StreamApdu_state(State.STOPPED);

                    ((TextView) getView().findViewById(
                            R.id.txtStreamApduReturnValue)).setText("");
                    ((TextView) getView()
                            .findViewById(R.id.txtStreamApduResult))
                            .setText("");

                    return;
                }

                ((ProgressBar) getView().findViewById(
                        R.id.progressBarStreamApdu))
                        .setVisibility(View.VISIBLE);

                TestFunc.INDIVIDUAL.set_StreamApdu_slotID(slot_id);
                TestFunc.INDIVIDUAL.set_StreamApdu_classType(class_type);
                TestFunc.INDIVIDUAL.set_StreamApdu_instruction(instruction);
                TestFunc.INDIVIDUAL.set_StreamApdu_param1(param1);
                TestFunc.INDIVIDUAL.set_StreamApdu_param2(param2);
                TestFunc.INDIVIDUAL.set_StreamApdu_param3(param3);
                TestFunc.INDIVIDUAL.set_StreamApdu_command(apdu_command);
                TestFunc.INDIVIDUAL.set_StreamApdu_state(State.RUNNING);

                // Run the task ...
                streamApdu_task = SAMExecutor.INSTANCE.streamApdu(
                        TestFunc.INDIVIDUAL,
                        int_slot_id,
                        int_class_type,
                        int_instruction,
                        int_param1,
                        int_param2,
                        int_param3,
                        apdu_command,
                        ((ProgressBar) getView().findViewById(
                                R.id.progressBarStreamApdu)));
                streamApdu_task.uihandler(new IndivTask.IHandler() {
                    @Override
                    public void refreshUI() {
                        int return_value = TestFunc.INDIVIDUAL
                                .get_StreamApdu_returnValue();
                        String result = TestFunc.INDIVIDUAL
                                .get_StreamApdu_result().toString();

                        ((TextView) getView().findViewById(
                                R.id.txtStreamApduReturnValue))
                                .setText(return_value + "");
                        ((TextView) getView().findViewById(
                                R.id.txtStreamApduResult)).setText(result);

                        ((FloatingActionButton) v)
                                .setImageResource(R.drawable.play);
                        TestFunc.INDIVIDUAL.set_StreamApdu_state(State.STOPPED);
                    }
                });
                streamApdu_task.execute();

            } else {
                ((FloatingActionButton) v).setImageResource(R.drawable.play);
                // ((ProgressBar)getView().findViewById(R.id.progressBarStreamApdu)).setVisibility(View.INVISIBLE);

                TestFunc.INDIVIDUAL.set_StreamApdu_state(State.STOPPED);

                // Stop the task ...
                streamApdu_task.cancel(true);
            }

            break;
        }
    }
}
