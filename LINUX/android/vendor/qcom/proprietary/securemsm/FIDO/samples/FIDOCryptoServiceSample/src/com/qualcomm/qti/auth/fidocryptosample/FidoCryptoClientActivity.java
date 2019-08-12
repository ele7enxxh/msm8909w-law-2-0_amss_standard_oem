/**
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.auth.fidocryptosample;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.RemoteException;
import android.text.format.Time;
import android.util.Base64;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;

import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.BindFidoCryptoButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.SetChallengeButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.BindAuthButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.ProcessButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.UnBindFidoCryptoButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.VerifyUserButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.onclicklistener.GetEnrollmentStatusButtonOnClickListener;
import com.qualcomm.qti.auth.fidocryptosample.persistence.PersistenceManager;

public class FidoCryptoClientActivity extends Activity {
  public static final String LOG_TAG = FidoCryptoClientActivity.class
      .getSimpleName();

  public static final int GET_INFO_RADIO_BUTTON_ID = 1;
  public static final int REGISTER_RADIO_BUTTON_ID = 2;
  public static final int DEREGISTER_RADIO_BUTTON_ID = 3;
  public static final int OPEN_SETTINGS_RADIO_BUTTON_ID = 4;
  public static final int SIGN_RADIO_BUTTON_ID = 5;
  public static final int MALFORMED_REQUEST_RADIO_BUTTON_ID = 6;

  private static final String FIDOCRYPTOSERVICE_PACKAGE = "com.qualcomm.qti.auth.fidocryptoservice";
  private static final String MOCKAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.mockauthservice";
  private static final String SAMPLEAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.sampleauthenticatorservice";
  private static final String SECSAMPLEAUTHSERVICE_PACKAGE = "com.qualcomm.qti.auth.securesampleauthservice";

  public FidoCryptoConnection fidoCryptoConnection;
  public MaConnection maConnection;
  public SAConnection saConnection;
  public SSAConnection ssaConnection;

  private IFidoCryptoListenerImpl iFidoCryptoListener;
  public IMockAuthListenerImpl iMockAuthListener;
  public SampleAuthenticatorListenerImpl iSampleAuthenticatorListener;
  public SecureSampleAuthListenerImpl iSecureSampleAuthListener;

  public String authenticatorName;

  public byte[] encapsulatedResult;

  public RadioGroup radioGroup;
  public Spinner spinner;

  FileOutputStream outputStream;

  public PersistenceManager persistenceManager;

  public static byte[] finalChallenge;

  public static void newChallenge() {
    Random r = new Random();
    finalChallenge = new byte[3];
    r.nextBytes(finalChallenge);
  }

  static {
    newChallenge();
  }

  public void bindFidoCryptoService() throws ServiceAlreadyBoundException {
    if (this.fidoCryptoConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(FIDOCRYPTOSERVICE_PACKAGE,
        FIDOCRYPTOSERVICE_PACKAGE + ".FidoCryptoService");
    bindService(bindIntent, this.fidoCryptoConnection, Context.BIND_AUTO_CREATE);
  }

  public void unbindFidoCryptoService() {
    unbindService(fidoCryptoConnection);
    fidoCryptoConnection.unbind();
  }

  public void bindMAService() throws ServiceAlreadyBoundException {
    if (this.maConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(MOCKAUTHSERVICE_PACKAGE, MOCKAUTHSERVICE_PACKAGE
        + ".MockAuthService");
    bindService(bindIntent, this.maConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSAService() throws ServiceAlreadyBoundException {
    if (this.saConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SAMPLEAUTHSERVICE_PACKAGE, SAMPLEAUTHSERVICE_PACKAGE
        + ".SampleAuthenticatorService");
    bindService(bindIntent, this.saConnection, Context.BIND_AUTO_CREATE);
  }

  public void bindSSAService() throws ServiceAlreadyBoundException {
    if (this.ssaConnection.isBound()) {
      throw new ServiceAlreadyBoundException();
    }
    Intent bindIntent = new Intent();
    bindIntent.setClassName(SECSAMPLEAUTHSERVICE_PACKAGE, SECSAMPLEAUTHSERVICE_PACKAGE
        + ".SecureSampleAuthService");
    bindService(bindIntent, this.ssaConnection, Context.BIND_AUTO_CREATE);
  }

  @Override
  public void onCreate(Bundle savedInstanceState) {
    Log.d(LOG_TAG, "onCreate()");
    Thread.currentThread().setName(
        FidoCryptoClientActivity.class.getSimpleName());
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);

    fidoCryptoConnection = new FidoCryptoConnection(this);
    maConnection = new MaConnection(this);
    saConnection = new SAConnection(this);
    ssaConnection = new SSAConnection(this);

    this.iFidoCryptoListener = new IFidoCryptoListenerImpl(this);
    this.iMockAuthListener = new IMockAuthListenerImpl(this);
    this.iSampleAuthenticatorListener = new SampleAuthenticatorListenerImpl(this);
    this.iSecureSampleAuthListener = new SecureSampleAuthListenerImpl(this);

    Button processButton = (Button) this.findViewById(R.id.processButton);
    ProcessButtonOnClickListener processButtonOnClickListener = new ProcessButtonOnClickListener(
        this);
    processButton.setOnClickListener(processButtonOnClickListener);

    Button bindFidoButton = (Button) this.findViewById(R.id.bindFidoButton);
    BindFidoCryptoButtonOnClickListener bindFidoButtonOnClickListener = new BindFidoCryptoButtonOnClickListener(
        this);
    bindFidoButton.setOnClickListener(bindFidoButtonOnClickListener);

    Button unbinFidoButton = (Button) this.findViewById(R.id.unbindFidoButton);
    UnBindFidoCryptoButtonOnClickListener unbindFidoButtonOnClickListener =
      new UnBindFidoCryptoButtonOnClickListener(this);
    unbinFidoButton.setOnClickListener(unbindFidoButtonOnClickListener);

    Button setChallengeButton = (Button) this.findViewById(R.id.setChallengeButton);
    SetChallengeButtonOnClickListener setChallengeButtonOnClickListener =
      new SetChallengeButtonOnClickListener(this);
    setChallengeButton.setOnClickListener(setChallengeButtonOnClickListener);

    Button bindAuthButton = (Button) this.findViewById(R.id.bindAuthButton);
    BindAuthButtonOnClickListener bindAuthButtonOnClickListener = new BindAuthButtonOnClickListener(
        this);
    bindAuthButton.setOnClickListener(bindAuthButtonOnClickListener);

    Button verifyUserButton = (Button) this.findViewById(R.id.verifyUserButton);
    VerifyUserButtonOnClickListener verifyUserButtonOnClickListener = new VerifyUserButtonOnClickListener(
        this);
    verifyUserButton.setOnClickListener(verifyUserButtonOnClickListener);

    Button getEnrollmentStatusButton = (Button) this.findViewById(R.id.getEnrollmentStatusButton);
    GetEnrollmentStatusButtonOnClickListener getEnrollmentStatusOnClickListener = new GetEnrollmentStatusButtonOnClickListener(
      this);
    getEnrollmentStatusButton.setOnClickListener(getEnrollmentStatusOnClickListener);

    radioGroup = (RadioGroup) this.findViewById(R.id.radioGroup1);
    ((RadioButton) this.findViewById(R.id.radio0))
        .setId(FidoCryptoClientActivity.GET_INFO_RADIO_BUTTON_ID);
    ((RadioButton) this.findViewById(R.id.radio1))
        .setId(FidoCryptoClientActivity.REGISTER_RADIO_BUTTON_ID);
    ((RadioButton) this.findViewById(R.id.radio2))
        .setId(FidoCryptoClientActivity.DEREGISTER_RADIO_BUTTON_ID);
    ((RadioButton) this.findViewById(R.id.radio3))
        .setId(FidoCryptoClientActivity.OPEN_SETTINGS_RADIO_BUTTON_ID);
    ((RadioButton) this.findViewById(R.id.radio4))
        .setId(FidoCryptoClientActivity.SIGN_RADIO_BUTTON_ID);
    ((RadioButton) this.findViewById(R.id.radio5))
        .setId(FidoCryptoClientActivity.MALFORMED_REQUEST_RADIO_BUTTON_ID);

    spinner = (Spinner) findViewById(R.id.authenticator_spinner);
    ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
      R.array.authenticator_array, android.R.layout.simple_spinner_item);
    adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
    spinner.setAdapter(adapter);

    persistenceManager = new PersistenceManager(getBaseContext());
    try {
      outputStream = getApplicationContext().openFileOutput("logfile",
          Context.MODE_PRIVATE);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  public void setText(String string) {
    TextView textView = (TextView) this.findViewById(R.id.messageTextView);
    textView.setText(string);
  }

  public void showResponse(byte[] uafResponse) {
    StringBuilder stringBuilder = new StringBuilder();
    for (int i = 0; i < 4 && i < uafResponse.length; i++) {
      stringBuilder.append(String.format("0x%02X ", uafResponse[i]));
    }
    this.setText(stringBuilder.toString());
  }

  public int process(byte[] command, byte[] cookie) throws RemoteException {
    try {
      Time now = new Time(Time.getCurrentTimezone());
      now.setToNow();
      String timeString = now.format("\n\n%T: request: ");
      outputStream.write(timeString.getBytes());
      outputStream.write(Base64.encodeToString(command, Base64.DEFAULT)
          .getBytes());
      outputStream.write("\n".getBytes());
    } catch (IOException e) {
    }
    return this.fidoCryptoConnection.process(command, "app0", cookie,
        iFidoCryptoListener);
  }

  public int setChallenge() throws RemoteException {
    return this.fidoCryptoConnection.setChallenge(finalChallenge,
        iFidoCryptoListener);
  }

  @Override
  public void onDestroy() {
    if (fidoCryptoConnection.isBound()) {
      unbindFidoCryptoService();
    }
    if (maConnection.isBound()) {
      unbindService(maConnection);
      maConnection.unbound();
    }
    if (saConnection.isBound()) {
      unbindService(saConnection);
      saConnection.unbound();
    }
    if (ssaConnection.isBound()) {
      unbindService(ssaConnection);
      ssaConnection.unbound();
    }
    try {
      if (outputStream != null) {
        outputStream.close();
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
    super.onDestroy();
  }

}
