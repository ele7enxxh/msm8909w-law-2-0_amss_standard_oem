/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 */

package org.codeaurora.ims.parser;

import android.content.Context;
import android.telecom.VideoProfile;
import android.telephony.SubscriptionManager;
import android.util.Log;
import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.codeaurora.ims.QtiViceInfo;

/*
 * ImsViceParser will maintain context of previous Vice info element and present element
 * to do further validation part before updating Vice data to UI.
 */
public class ImsViceParser extends ConfInfo {
    // Member variable
    private Context mContext;

    // Local variables
    private static ViceSaxXmlHandler sHandler;
    private String mViceDialogStr = null;
    private List<DialogInfo> mDialogIds;
    private static ArrayList<Element> mDialogList;
    private ViceSaxXmlParser mViceparser;
    private String LOGTAG = "ImsViceParser";
    private static final String LOCAL_TARGET_PARAM_NAME = "+sip.rendering";
    private static final String LOCAL_TARGET_PARAM_PVAL_NO = "no";

    //DialogIds structure will hold all required information
    //for a particular dialog
    public static class DialogInfo {
        public String dialogId;
        public String callId;
        public String direction;
        public String exclusive;
        public String state;
        public String paramName;
        public String paramVal;
        public String remoteIdentity;
        public List<MediaAttribute> mediaAttributes = new ArrayList<>();
    }

    //MediaAttribute structure will hold media attributes
    //for a particular DialogId
    public static class MediaAttribute {
        public String mediaType;
        public String mediaDirection;
        public String mediaPort;
    }

    public ImsViceParser(Context context) {
        mContext = context;
        preparePartialList();
    }

    public static void setSAXHandler(ViceSaxXmlHandler handler) {
        sHandler = handler;
    }

    public void updateViceXmlBytes(byte[] vicexml) {
        InputStream is = new ByteArrayInputStream(vicexml);
        mDialogIds = new ArrayList<>();
        try {
            mViceDialogStr = new String(vicexml, "UTF-8");
            Log.d(LOGTAG,"VICE XML in string = " + mViceDialogStr);
            mViceparser = ViceSaxXmlParser.getInstance();
            mNewElement = mViceparser.parse(is);
            debugLog("*************New Vice Notification*****************");
            updateNotification(null, mCachedElement, mNewElement, -1);
            mDialogIds = getDialogInfo();
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /*
     * If state not present to a tag , default state will be full otherwise
     * resultant state will be value of new element of corresponding new
     * notification
     */
    private String checkElementState(Element element) {
        String resultantState = Element.STATE_FULL;
        String elementState;
        elementState = element.getAttributeValue(Element.VICE_DIALOG_INFO_STATE);
        if (elementState != null) {
            resultantState = elementState;
        }
        debugLog("Notification state is" + resultantState + " element state " + elementState);
        return resultantState;
    }

    /*
     * Look up table to maintain table partial state supporting tags
     */
    private void preparePartialList() {
        allowedPartial.add(Element.VICE_DIALOG_STATE);
    }

    public List<DialogInfo> getDialogInfo() {
        return mViceparser.getDialogInfo();
    }

    public String getViceDialogInfoAsString() {
        return mViceDialogStr;
    }

    /**
     * Audio      Video         port        Type of Call    Pullable
     * ==============================================================
     * sendrcv    NA/anyvalue    NA/Present  VolteActive     Yes
     * sendonly   NA/anyvalue    NA/Present  VolteHeld       No
     * rcvonly    NA/anyvalue    NA/Present  VolteHeld       No
     * inactive   NA/anyvalue    NA/Present  VolteHeld       No
     * sendrcv    sendrcv        NA          VT-TX-RX        Yes
     * sendrcv    sendonly       NA          VT-TX           Yes
     * sendrcv    rcvonly        NA          VT-RX           Yes
     * sendrcv    inactive       NA          VTHeld          No
     * sendonly   anyvalue       NA          VTHeld          No
     * rcvonly    anyvalue       NA          VTHeld          No
     * inactive   anyvalue       NA          VTHeld          No
     * =============================================================
     */
    private String getCallType(DialogInfo dialog) {
        String value = null;
        String audioState = null;
        String videoState = null;
        boolean portPresent = false;

        if (dialog != null && dialog.state.equalsIgnoreCase(QtiViceInfo.STATE_CONFIRMED)) {
            int size = dialog.mediaAttributes.size();
            for (int i = 0; i < size; i++) {
                MediaAttribute media = dialog.mediaAttributes.get(i);
                if ((media != null) && (media.mediaType != null) &&
                        (media.mediaDirection != null)) {
                    if (media.mediaType.equalsIgnoreCase(QtiViceInfo.MEDIA_TYPE_AUDIO)) {
                        if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_SENDRECV)) {
                            audioState = QtiViceInfo.CALL_TYPE_VOICE_ACTIVE;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_SENDONLY) ||
                                media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_RECVONLY) ||
                                media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_INACTIVE)) {
                            audioState = QtiViceInfo.CALL_TYPE_VOICE_HELD;
                        }
                    } else if (media.mediaType.equalsIgnoreCase(QtiViceInfo.MEDIA_TYPE_VIDEO)) {
                        if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_SENDRECV)) {
                            videoState = QtiViceInfo.CALL_TYPE_VIDEO_TX_RX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_SENDONLY)) {
                            videoState = QtiViceInfo.CALL_TYPE_VIDEO_TX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_RECVONLY)) {
                            videoState = QtiViceInfo.CALL_TYPE_VIDEO_RX;
                        } else if (media.mediaDirection.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_DIRECTION_INACTIVE)) {
                            videoState = QtiViceInfo.CALL_TYPE_VIDEO_HELD;
                        }
                        if ((media.mediaType.equalsIgnoreCase(
                                QtiViceInfo.MEDIA_TYPE_VIDEO)) && (media.mediaPort != null) &&
                                (!media.mediaPort.isEmpty())) {
                            portPresent = true;
                        }
                    }
                } else {
                    Log.e(LOGTAG, "Media Parameter incorrect!!");
                }
                Log.d(LOGTAG, "audioState = " + audioState + ", videoState = " + videoState
                        + ", portPresent = " + portPresent);
            }

            if (audioState != null) {
                if (audioState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VOICE_ACTIVE)) {
                    if ((videoState == null) || ((videoState != null) && portPresent)) {
                        return QtiViceInfo.CALL_TYPE_VOICE_ACTIVE;
                    } else if (videoState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_TX_RX)) {
                        return QtiViceInfo.CALL_TYPE_VIDEO_TX_RX;
                    } else if (videoState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_TX)) {
                        return QtiViceInfo.CALL_TYPE_VIDEO_TX;
                    } else if (videoState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_RX)) {
                        return QtiViceInfo.CALL_TYPE_VIDEO_RX;
                    } else if (videoState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_HELD)) {
                        return QtiViceInfo.CALL_TYPE_VIDEO_HELD;
                    }
                } else if (audioState.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VOICE_HELD)) {
                    if ((videoState == null) || ((videoState != null) && portPresent)) {
                        return QtiViceInfo.CALL_TYPE_VOICE_HELD;
                    } else if ((videoState != null) && !portPresent) {
                        return QtiViceInfo.CALL_TYPE_VIDEO_HELD;
                    }
                }
            } else {
                Log.e(LOGTAG, "audioState null!!");
            }
        }
        return null;
    }

    /**
     * Call should be pullable when
     * In XML, Sa:Exclusive = false
     * Feature capability is present
     * When state is either active/sendonly/recvonly
     * RFC 4235 Section 5.2, call cannot be pulled if
     * If paramname present, its value is "+sip.rendering" and
     * corresponding paramval value is "no" - this implies call is on hold
     * if paramval is "yes", then call pullable
     */
    private String isCallPullable(DialogInfo dialog,
                                  boolean isVideoSupported, boolean isVolteSupported) {
        String pullable = String.valueOf(false);
        boolean canVtBePulled = false;
        if (dialog != null) {
            String callType = getCallType(dialog);
            if (callType != null) {
                if (callType.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_TX_RX) ||
                        callType.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_TX) ||
                        callType.equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VIDEO_RX)) {
                    canVtBePulled = true;
                }
                if ((dialog.exclusive.equalsIgnoreCase("false")) &&
                        ((getCallType(dialog).equalsIgnoreCase(QtiViceInfo.CALL_TYPE_VOICE_ACTIVE)
                                && isVolteSupported) ||
                                (canVtBePulled && isVideoSupported))) {
                    pullable = String.valueOf(true);

                    if (dialog.paramName != null &&
                            dialog.paramName.equalsIgnoreCase(LOCAL_TARGET_PARAM_NAME) &&
                            dialog.paramVal != null &&
                            dialog.paramVal.equalsIgnoreCase(LOCAL_TARGET_PARAM_PVAL_NO)) {
                        pullable = String.valueOf(false);
                    }

                }
            }
        }
        return pullable;
    }

    public List<String[]> getCallPullInfo(boolean isVideoSupported, boolean isVolteSupported) {
        List<String[]> dialogInfo = new ArrayList<String[]>();

        if (isDialogIdsEmpty()) {
            Log.d(LOGTAG, "getCallPullInfo mDialogIds null");
            return null;
        }

        for (int i = 0; i < mDialogIds.size(); i++) {
            String[] callPull = new String[QtiViceInfo.INDEX_MAX];
            callPull[QtiViceInfo.INDEX_DIALOG_ID] = mDialogIds.get(i).dialogId;
            callPull[QtiViceInfo.INDEX_NUMBER] = mDialogIds.get(i).remoteIdentity;
            callPull[QtiViceInfo.INDEX_CALLTYPE] = getCallType(mDialogIds.get(i));
            callPull[QtiViceInfo.INDEX_ISPULLABLE] = isCallPullable(mDialogIds.get(i),
                    isVideoSupported, isVolteSupported);
            callPull[QtiViceInfo.INDEX_DIRECTION] = mDialogIds.get(i).direction;

            dialogInfo.add(callPull);

            Log.d(LOGTAG, "getCallPullInfo [" + i + "] - "
                + ", DialogId = " + callPull[QtiViceInfo.INDEX_DIALOG_ID]
                + ", Number = " + callPull[QtiViceInfo.INDEX_NUMBER]
                + ", Is Pullable = " + callPull[QtiViceInfo.INDEX_ISPULLABLE]
                + ", Call Type = " + callPull[QtiViceInfo.INDEX_CALLTYPE]
                + ", Direction = " + callPull[QtiViceInfo.INDEX_DIRECTION]);
        }
        return dialogInfo;
    }

    private boolean isDialogIdsEmpty() {
        return (mDialogIds == null || mDialogIds.isEmpty());
    }
}
