/*
 * Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.qti.soundtrigger;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;

import com.qualcomm.listen.ListenTypes.SoundModelInfo;
import com.qualcomm.listen.ListenTypes.VWUKeywordConfLevel;
import com.qualcomm.listen.ListenTypes.KeywordInfo;
import com.qualcomm.listen.ListenTypes.SVASoundModelInfo;
import com.qualcomm.listen.ListenTypes.VWUUserKeywordPairConfLevel;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.util.Log;

public class SoundModelRepository {
    private static final String TAG = "ListenLog.SoundModelRepository";
    private final static String KEYPHRASE_SOUNDMODEL_FILE_EXT = ".uim";
    private final static String USER_SOUNDMODEL_FILE_EXT = ".udm";
    private final static String TAG_DELIMITER = "|";

    ArrayList<SoundModel> soundModels  = new ArrayList<SoundModel>();
    private String selectedSoundModelName = null;
    private String soundModelNameToQuery = null;
    private String soundModelNameToExtend = null;
    private String soundModelNameToDeleteFrom = null;
    private String mergedSoundModelName = null;
    private ByteBuffer extendedSoundModel = null;
    private boolean isASessionRegistered = false;
    private ByteBuffer soundModelToModifyByteBuffer = null;
    ArrayList<ByteBuffer> soundModelsToMergeByteBuffers = new ArrayList<ByteBuffer>();
    private ArrayList<String> smNamesToRegister = null;
    private ArrayList<String> loadedSmNames = null;

    SoundModelRepository() {
        //nada
    }

    // Gets all sound models from the file system
    public ArrayList<SoundModel> getAllSoundModels() {
        Log.v(TAG, "getAllSoundModels");

        //TODO instead of clearing the list each time, compare the file system sound model
        // to the one in memory. If equal, do nothing. If not equal, replace in memory. Remove
        // all in memory that aren't on the file system.

        // remove all SoundModels from the existing list
        //soundModels.clear();

        // If there exists sound models in the list, just return the list instead of building it
        // over again.
        if (false == soundModels.isEmpty()) {
            return soundModels;
        }

        File dir = new File(Global.PATH_APP);

        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                if (filename.endsWith(KEYPHRASE_SOUNDMODEL_FILE_EXT) ||
                        filename.endsWith(USER_SOUNDMODEL_FILE_EXT)) {
                    return true;
                }
                return false;
            }
        });
        if (null == files) {
            Log.e(TAG, "getAllSoundModels: no sound models found");
            return null;
        }


        boolean foundSelectedSoundModel = false;
        for(File file : files) {
            String soundModelFileName = file.getName();

            if (soundModelFileName.contains(".vwu")) {
                Log.e(TAG, "getAllSoundModels: Error- v1.0 SM detected");
                continue;
            } else if (soundModels.contains(new SoundModel(soundModelFileName))) {
                Log.v(TAG, "getAllSoundModels: duplicate sound model name: " + soundModelFileName +
                        " skipping adding SM and continuing to next SM");
                continue;
            } else if (null != this.selectedSoundModelName && this.selectedSoundModelName.equals(
                    soundModelFileName)){
                foundSelectedSoundModel = true;
            }

            Log.v(TAG, "getAllSoundModels: adding soundModelName= " + soundModelFileName);
            soundModels.add(new SoundModel(soundModelFileName));
        }

        if (false == foundSelectedSoundModel) {
            this.selectedSoundModelName = null;
        }

        Collections.sort(soundModels);
        return soundModels;
    }

    // if boolean arg is false, get all the user made sound models
    public ArrayList<String> getSoundModelNames(final boolean onlyKeyPhaseSoundModels) {
        Log.v(TAG, "getSoundModelNames");
        File dir = new File(Global.PATH_APP);
        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String filename) {
                if (onlyKeyPhaseSoundModels) {
                    return filename.endsWith(KEYPHRASE_SOUNDMODEL_FILE_EXT);
                } else {
                    return filename.endsWith(USER_SOUNDMODEL_FILE_EXT) ||
                            filename.endsWith(KEYPHRASE_SOUNDMODEL_FILE_EXT);
                }
            }
        });

        if (null == files) {
            Log.e(TAG, "getSoundModelNames: no sound models found");
            return null;
        }

        ArrayList<String> soundModelArrayList = new ArrayList<String>();
        String soundModelFileName;
        String keyPhraseName = "";
        for (int i = 0; i < files.length; i++) {
            soundModelFileName = files[i].getName();
            if (onlyKeyPhaseSoundModels) {
                keyPhraseName = soundModelFileName.substring(0, soundModelFileName.lastIndexOf('.'));
                Log.v(TAG, "getSoundModelNames: keyPhraseName= " + keyPhraseName);
                if (false == soundModelArrayList.contains(keyPhraseName)) {
                    soundModelArrayList.add(keyPhraseName);
                } else {
                    Log.e(TAG, "getSoundModelNames: keyPhraseName= " + keyPhraseName + " is a duplicate.");
                }
            } else {
                if (false == soundModelArrayList.contains(soundModelFileName)) {
                    soundModelArrayList.add(soundModelFileName);
                } else {
                    Log.e(TAG, "getSoundModelNames: soundModelFileName= " + soundModelFileName +
                            " is a duplicate.");
                }
            }
        }

        return soundModelArrayList;
    }


    public String getSmPrettyName(String uglyName) {
        return uglyName.substring(0, uglyName.indexOf("."));
    }

    /***
     * Can't use because we don't usually have all Sound Models queried,
     * and therefore don't usually know all users.
     *
     * @return string array of users
     */
    public String[] getAllUsers() {
        Log.v(TAG, "getAllUsers");

        ArrayList<String> userArrayList = new ArrayList<String>();
        for (SoundModel soundModel : soundModels) {
            for (Keyphrase keyPhrase : soundModel.getKeyphrases()) {
                for (User user : keyPhrase.getUsers()) {
                    if (false == userArrayList.contains(user.name)) {
                        userArrayList.add(user.name);
                    }
                }
            }
        }
        Collections.sort(userArrayList);
        String[] userArray = userArrayList.toArray(new String[userArrayList.size()]);
        return userArray;
    }



    public VWUKeywordConfLevel[] getKeyphraseConfidenceThresholdsBySmName(Context context,
            String smName) {
        Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName);
        SoundModel selectedSoundModel = getSmByName(smName);
        if (null == selectedSoundModel) {
            Log.e(TAG, "getKeyphraseConfidenceThresholdsBySmName: cannot get SM for smName= " + smName);
            return null;
        }
        ArrayList<VWUKeywordConfLevel> keywordConfLevels = new ArrayList<VWUKeywordConfLevel>();
        for (Keyphrase keyword : selectedSoundModel.getKeyphrases()) {
            VWUKeywordConfLevel keywordConfLevel = new VWUKeywordConfLevel();
            keywordConfLevel.keyword = keyword.getName();
            keywordConfLevel.confLevel = (short) getConfidenceLevel(context,
                                                                   selectedSoundModel.getName(),
                                                                   keyword.getName(),
                                                                   null);
            Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName +
                    ", keyword= " + keyword.getName() + ", confLevel= " + keywordConfLevel.confLevel);
            keywordConfLevels.add(keywordConfLevel);
        }
        return keywordConfLevels.toArray(new VWUKeywordConfLevel[keywordConfLevels.size()]);
    }

    public VWUUserKeywordPairConfLevel[] getUserConfidenceThresholdsBySmName(Context context,
            String smName) {
        Log.v(TAG, "getUserConfidenceThresholdsBySmName: smName= " + smName);
        SoundModel selectedSoundModel = getSmByName(smName);

        ArrayList<VWUUserKeywordPairConfLevel> userConfLevels = new ArrayList<VWUUserKeywordPairConfLevel>();
        for (Keyphrase keyword : selectedSoundModel.getKeyphrases()) {
            for (User user : keyword.getUsers()) {
                VWUUserKeywordPairConfLevel userConfLevel = new VWUUserKeywordPairConfLevel();
                userConfLevel.keyword = keyword.getName();
                userConfLevel.user = user.getName();
                userConfLevel.confLevel = (short) getConfidenceLevel(context,
                                                                    selectedSoundModel.getName(),
                                                                    keyword.getName(),
                                                                    user.getName());
                Log.v(TAG, "getKeyphraseConfidenceThresholdsBySmName: smName= " + smName +
                        ", keyword= " + keyword.getName() + ", userName= " + user.getName() +
                        ", confLevel= " + userConfLevel.confLevel);
                userConfLevels.add(userConfLevel);
            }
        }
        return userConfLevels.toArray(new VWUUserKeywordPairConfLevel[userConfLevels.size()]);
    }


    /*** Registered Sound Model Functions **/

    // Check if a sound model file exists by its keyword and username
    private boolean soundModelFileExists(String inSoundModelName) {
        File soundModelFile = new File(generateSoundModelFilePath(inSoundModelName));
        return soundModelFile.exists();
    }

    // Given a keyword and user name provide the name of the sound model
    private String generateSoundModelFilePath(String inSoundModelName) {
        Log.v(TAG, "generateSoundModelFilePath: path= " + Global.PATH_APP + "/" +
                inSoundModelName);
        return Global.PATH_APP + "/" + inSoundModelName;
    }

    public boolean isKeyphraseSoundModel(String inSoundModelName) {
        return inSoundModelName.contains(KEYPHRASE_SOUNDMODEL_FILE_EXT);
    }

    public boolean soundModelExists(String soundModelName) {
        return soundModelFileExists(soundModelName);
    }

    public String getUserSoundModelName(String soundModelName) {
        return soundModelName + USER_SOUNDMODEL_FILE_EXT;
    }

    public String getSystemSoundModelName(String soundModelName) {
        return soundModelName + KEYPHRASE_SOUNDMODEL_FILE_EXT;
    }

    public String getSoundModelNameFromLongName(String soundModelName) {
        return soundModelName.substring(0, soundModelName.lastIndexOf('.'));
    }

    // Returns true if the service needs to be called to merge to SMs
    public boolean createNewSoundModel(ArrayList<String> baseSoundModelNames, String newSoundModelName) {
        if (0 == baseSoundModelNames.size() || newSoundModelName == null) {
            Log.e(TAG, "createNewSoundModel: bad params");
        }

        if (1 == baseSoundModelNames.size()) {
            String sourceFilePath = Global.PATH_APP + "/" + baseSoundModelNames.get(0);
            String destFilePath = Global.PATH_APP + "/" + newSoundModelName;
            Utils.createNewSoundModel(sourceFilePath, destFilePath);
            soundModels.add(new SoundModel(newSoundModelName));
            return false;
        } else {
            this.soundModelsToMergeByteBuffers.clear();
            for (String soundModelToAddName : baseSoundModelNames) {
                if (soundModelExists(soundModelToAddName)) {
                    this.soundModelsToMergeByteBuffers.add(
                            getSoundModelByteBufferFromName(soundModelToAddName));
                } else {
                    Log.e(TAG, "createNewSoundModel: sound model: " + soundModelToAddName +
                            " does not exist");
                }
            }
            setMergedSoundModelName(newSoundModelName);
            return true;
        }
    }

    public ArrayList<ByteBuffer> getSoundModelsToMergeByteBuffers() {
        return this.soundModelsToMergeByteBuffers;
    }

    public void setSoundModelNameToQuery(String inSoundModelName) {
        this.soundModelNameToQuery  = inSoundModelName;
        Log.v(TAG, "setSoundModelNameToQuery: sound model name= " + soundModelNameToQuery);
    }

    public ByteBuffer getSoundModelByteBufferToQuery() {
        return getSoundModelByteBufferFromName(this.soundModelNameToQuery);
    }

    public SoundModel getQueriedSoundModel() {
        if (null == this.soundModelNameToQuery) {
            return null;
        }
        return getSmByName(this.soundModelNameToQuery);
    }

    public SoundModel getSmByName(String inSoundModelName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "getSoundModelByName: SM passed in is null");
            return null;
        }
        for (SoundModel soundModel : soundModels) {
            //Log.v(TAG, "getSoundModelByName: testing SM= " + soundModel.getName());
            if (soundModel.equals(new SoundModel(inSoundModelName))) {
                return soundModel;
            }
        }
        Log.e(TAG, "getSoundModelByName: SM= " + inSoundModelName + " does not exist");
        return null;
    }

    public void addSmInfoFromQuery(SoundModelInfo smInfo, ArrayList<String> userKeyphrasePairIndices) {
        Log.v(TAG, "addSmInfoFromQuery: Adding info for SM: " + this.soundModelNameToQuery);
        SVASoundModelInfo v2SoundModelInfo = (SVASoundModelInfo) smInfo;
        KeywordInfo[] kwInfos = v2SoundModelInfo.keywordInfo;

        //temp code
        boolean fillIndices = true;
        if (userKeyphrasePairIndices == null) {
            fillIndices = false;
        }

        // Build the sound model from the KeywordInfo[] object.
        SoundModel soundModelToAdd = new SoundModel(this.soundModelNameToQuery);
        Log.v(TAG, "addSmInfoFromQuery: Adding SM: " + this.soundModelNameToQuery + " with: " +
                kwInfos.length + " keywords.");
        int keyphraseIndex = -2; // -1 is already taken for index not found.
        int userIndex = -2; // -1 is already taken for index not found.
        for (KeywordInfo kwInfo : kwInfos) {
            Keyphrase keyphraseToAdd;
            if (fillIndices) { //temp code
                keyphraseIndex = userKeyphrasePairIndices.indexOf(kwInfo.keywordPhrase);
                Log.v(TAG, "addSmInfoFromQuery: found keyphraseIndex= " + keyphraseIndex);
                // Error checking. If userKeyphrasePairIndices doesn't contain kwInfo.keywordPhrase.
                if (keyphraseIndex == -1) {
                    Log.e(TAG, "addSmInfoFromQuery: userKeyphrasePairIndices doesn't contain " +
                            "kwInfo.keywordPhrase= " + kwInfo.keywordPhrase);
                }
                keyphraseToAdd = new Keyphrase(kwInfo.keywordPhrase, keyphraseIndex);
            } else { //temp code
                keyphraseToAdd = new Keyphrase(kwInfo.keywordPhrase, -3); //temp code
            } //temp code
            Log.v(TAG, "addSmInfoFromQuery: Building SM: " + this.soundModelNameToQuery +
                    " adding keyphrase: " + kwInfo.keywordPhrase + " with ID= " + keyphraseIndex);
            for (String userName : kwInfo.activeUsers) {
                if (fillIndices) { //temp code
                    userIndex = userKeyphrasePairIndices.indexOf(userName + "|"
                            + kwInfo.keywordPhrase);
                    Log.v(TAG, "addSmInfoFromQuery: found userIndex= " + userIndex + " for " +
                            "user|keyphrase= " + userName + "|" + kwInfo.keywordPhrase);
                    // Error checking. If userKeyphrasePairIndices doesn't contain kwInfo.keywordPhrase.
                    if (userIndex == -1) {
                        Log.e(TAG, "addSmInfoFromQuery: userKeyphrasePairIndices doesn't contain " +
                                "user|keyphrase= " + userName + "|" + kwInfo.keywordPhrase);
                    }
                    keyphraseToAdd.addUser(userName, userIndex);
                } else { //temp code
                    keyphraseToAdd.addUser(userName, -3); //temp code
                } //temp code
                Log.v(TAG, "addSmInfoFromQuery: Building SM: " + this.soundModelNameToQuery +
                        " building keyword: " + kwInfo.keywordPhrase + " adding user: " + userName);
            }
            soundModelToAdd.addKeyphrase(keyphraseToAdd);
        }

        // if the sound model isn't already in the list, add it
        if (false == soundModels.contains(new SoundModel(this.soundModelNameToQuery))) {
            soundModels.add(soundModelToAdd);
            Log.v(TAG, "addSmInfoFromQuery: Added SM: " + this.soundModelNameToQuery);
        } else {
            //TODO add if test to test whether SMs are equal, then break;
            // the sound model is already in the list, write over existing sound model
            Log.v(TAG, "addSmInfoFromQuery: SM: " + this.soundModelNameToQuery +
                    " already exists, writing over exiting");
            Log.v(TAG, "addSmInfoFromQuery: printSms()= " + printSms());
            for (SoundModel soundModel : soundModels) {
                Log.v(TAG, "addSmInfoFromQuery: testing SM: " + soundModel.getName());
                if (soundModel.equals(soundModelToAdd)) {
                    Log.v(TAG, "addSmInfoFromQuery: found SM: " + soundModel.getName());
                    Log.v(TAG, "addSmInfoFromQuery: " + printSms());
                    if (soundModels.remove(soundModel)) {
                        Log.v(TAG, "addSmInfoFromQuery: removed SM: " + soundModel.getName());
                        soundModels.add(soundModelToAdd);
                        Collections.sort(soundModels);
                        Log.v(TAG, "addSmInfoFromQuery: " + printSms());
                    } else {
                        Log.e(TAG, "addSmInfoFromQuery: could not be remove SM: " + soundModel.getName());
                    }
                    break;
                }
            }
        }

        //TODO remove after debugging done
        /*String smString = "";
        for (KeywordInfo kwi : kwis) {
            smString += kwi.keywordPhrase + "keyword: ";
            for (String user : kwi.activeUsers) {
                smString += user + " ";
            }
        }
        Log.v(TAG, "soundModelInfo: " + smString);*/
    }


    /*** Session-related functions ***/


    public void setLoadedSmNames(ArrayList<String> inLoadedSmNames) {
        this.loadedSmNames = inLoadedSmNames;
        //TODO transitions remove when working, debug only
        for (int i=0; i<this.loadedSmNames.size(); i++) {
            Log.v(TAG, "setLoadedSmNames: loadedSmNames.get(" + i + ")= " +
                    this.loadedSmNames.get(i));
        }
    }

    public ArrayList<String> getLoadedSmNames() {
        return this.loadedSmNames;
    }


    /*** SMDetailsActivity Functions **/

    public void toggleLaunchPreference(Context context, String inSoundModelName, String inKeywordName,
            String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get launch preference because sound model name was null");
            return;
        }
        SoundModel launchPreferenceSoundModel = getSmByName(inSoundModelName);
        if (null == launchPreferenceSoundModel) {
            Log.e(TAG, "Could not get launch preference because sound model does not exist");
            return;
        }

        String launchPreferenceTag = generateSettingsTag(SoundModel.Setting.LaunchPreference,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        boolean launchPreference = sp.getBoolean(launchPreferenceTag, false);
        Log.v(TAG, "launchPreference for SM: " + inSoundModelName + ":" + inKeywordName +
                ":" + inUserName + " was: " + launchPreference);
        Editor editor = sp.edit();
        editor.putBoolean(launchPreferenceTag, !launchPreference);
        editor.commit();
    }

    public boolean getLaunchPreference(Context context, String inSoundModelName, String inKeywordName,
            String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get launch preference because sound model name was null");
            return false;
        }
        SoundModel launchPreferenceSoundModel = getSmByName(inSoundModelName);
        if (null == launchPreferenceSoundModel) {
            Log.e(TAG, "Could not get launch preference because sound model does not exist");
            return false;
        }

        String launchPreferenceTag = generateSettingsTag(SoundModel.Setting.LaunchPreference,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        boolean launchPreference = sp.getBoolean(launchPreferenceTag, false);
        Log.v(TAG, "launchPreference for SM: " + inSoundModelName + ":" + inKeywordName +
                ":" + inUserName + "=" + launchPreference);
        return launchPreference;
    }

    public void setConfidenceLevel(Context context, String inSoundModelName, String inKeywordName,
            String inUserName, int inConfidenceLevel) {
        String confidenceLevelTag = generateSettingsTag(SoundModel.Setting.ConfidenceThreshold,
                inSoundModelName, inKeywordName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        Editor editor = sp.edit();
        editor.putInt(confidenceLevelTag, inConfidenceLevel);
        editor.commit();
    }

    // returns -1 for failure
    public int getConfidenceLevel(Context context, String inSoundModelName, String inKeyphraseName,
            String inUserName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Could not get confidence level because sound model name was null");
            return -1;
        }
        SoundModel confLevelSoundModel = getSmByName(inSoundModelName);
        if (null == confLevelSoundModel) {
            Log.e(TAG, "Could not get confidence level because sound model does not exist");
            return -1;
        }

        String confidenceLevelTag = generateSettingsTag(SoundModel.Setting.ConfidenceThreshold,
                inSoundModelName, inKeyphraseName, inUserName, null);
        SharedPreferences sp = context.getSharedPreferences(Global.PREFERENCE_GROUP_NAME, 0);
        int confidenceLevel = -1;
        if (null == inUserName) {
            confidenceLevel = sp.getInt(confidenceLevelTag, Global.getInstance().
                    getSettingKeyPhraseConfidenceLevel());
        } else {
            confidenceLevel = sp.getInt(confidenceLevelTag, Global.getInstance().
                    getSettingUserConfidenceLevel());
        }
        Log.v(TAG, "getConfidenceLevel: confidenceLevel for SM: " + inSoundModelName + ":" +
                inKeyphraseName + ":" + inUserName + "=" + confidenceLevel);
        return confidenceLevel;
    }

    // session uses: setting, and inSession
    // launch and confidenceThreshold use: setting, inSmName, inKwName, inUName
    // unused input params should be passed as null
    private String generateSettingsTag( SoundModel.Setting setting,
                                        String inSoundModelName,
                                        String inKeywordName,
                                        String inUserName,
                                        Integer inSessionNum) {
        String specificSetting = "";

        switch (setting) {
            case Session:
                specificSetting = "session";
                return Global.PREFERENCE_TAG_NAME + TAG_DELIMITER + specificSetting +
                        TAG_DELIMITER + inSessionNum;
            case LaunchPreference:
                specificSetting = "launch";
                break;
            case ConfidenceThreshold:
                specificSetting = "conf";
                break;
            default:
                break;
        }

        // for launch preference and confidence threshold
        return Global.PREFERENCE_TAG_NAME + TAG_DELIMITER + specificSetting + TAG_DELIMITER +
                inSoundModelName + TAG_DELIMITER + inKeywordName + TAG_DELIMITER + inUserName;
    }


    /*** Extend Functions ***/

    public void setSoundModelNameToExtend(String inSoundModelName) {
        this.soundModelNameToExtend = inSoundModelName;
    }

    public ByteBuffer getSoundModelByteBufferToExtend() {
        return getSoundModelByteBufferFromName(this.soundModelNameToExtend);
    }

    //TODO remove this and use SMBBtoModify like delete is
    // Creates a temporary sound model once the extended sound model size is known.
    public void createExtendedSoundModelByteBuffer(int size) {
        this.extendedSoundModel = ByteBuffer.allocate(size);
    }

    // Gets temporary sound model byte buffer created by createExtendedSoundModelByteBuffer()
    public ByteBuffer getExtendedSoundModel() {
        if (null == this.extendedSoundModel) {
            Log.v(TAG, "getExtendedSoundModel: must call setExtendedSoundModel before get");
        }
        return this.extendedSoundModel ;
    }

    // Saves the selected sound model to a file on the internal storage
    public void saveExtendedSoundModel() {
        Log.v(TAG, "saveExtendedSoundModel");

        //TODO uncomment this part
        /*//TODO remove this when initialization SM checking is completed
        for (SoundModel soundModel : soundModels) {
            if (soundModel.getName().equals(soundModelNameToExtend)) {
                soundModel.addUser(trainingKeyword, trainingUser);
            }
        }*/

        String filePath = generateSoundModelFilePath(soundModelNameToExtend);
        Log.v(TAG, "outputExtendedSoundModel: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.extendedSoundModel, filePath);
    }


    /*** DeleteData (delete user or keyword from sound model) ***/

    public boolean deleteSoundModel(String inSoundModelName) {
        if (null == inSoundModelName) {
            Log.e(TAG, "Sound model with name: " + inSoundModelName + " does not exist.");
            return false;
        }

        String filePath = generateSoundModelFilePath(inSoundModelName);
        Log.v(TAG, "deleteSoundModel: filePath= " + filePath);
        File soundModelFile = new File(filePath);
        if (soundModelFile.exists()) {
            if (soundModelFile.delete()) {
                Log.v(TAG, "deleteSoundModel: succeeded");
                return true;
            } else {
                Log.e(TAG, "deleteSoundModel: delete failed");
            }
        } else {
            Log.e(TAG, "deleteSoundModel: sound model does not exist");
        }
        return false;
    }

    public void setSoundModelNameToDeleteFrom(String inSoundModelName) {
        this.soundModelNameToDeleteFrom = inSoundModelName;
    }

    public ByteBuffer getSoundModelByteBufferToDeleteFrom() {
        return getSoundModelByteBufferFromName(this.soundModelNameToDeleteFrom);
    }

    public void createSoundModelToModifyByteBuffer(int size) {
        this.soundModelToModifyByteBuffer = ByteBuffer.allocate(size);
    }

    public ByteBuffer getSoundModelToModifyByteBuffer() {
        if (null == this.soundModelToModifyByteBuffer) {
            Log.v(TAG, "getExtendedSoundModel: must call setExtendedSoundModel before get");
        }
        return this.soundModelToModifyByteBuffer;
    }

    public void saveSoundModelToDeleteFrom() {
        Log.v(TAG, "saveSoundModelToDeleteFrom");
        String filePath = generateSoundModelFilePath(this.soundModelNameToDeleteFrom);
        Log.v(TAG, "outputSoundModelToDeleteFrom: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.soundModelToModifyByteBuffer, filePath);
    }


    /*** Merge Functions ***/

    public void setMergedSoundModelName(String inSoundModelName) {
        this.mergedSoundModelName = inSoundModelName;
    }

    public String getMergedSoundModelName() {
        return this.mergedSoundModelName;
    }

    public void saveMergedSoundModel() {
        Log.v(TAG, "saveMergedSoundModel");
        String filePath = generateSoundModelFilePath(this.mergedSoundModelName);
        Log.v(TAG, "saveMergedSoundModel: filePath= " + filePath);
        Utils.saveByteBufferToFile(this.soundModelToModifyByteBuffer, filePath);
    }


    /*** Utility Functions ***/

    // returns null if no SM with specified name exists
    public ByteBuffer getSoundModelByteBufferFromName(String inSoundModelName) {
        if (soundModelFileExists(inSoundModelName)) {
            try {
                ByteBuffer soundModel = Utils.readFileToByteBuffer(
                        generateSoundModelFilePath(inSoundModelName));
                return soundModel;
            } catch (IOException e) {
                Log.e(TAG, "getKeywordOnlySoundModel: failed to read sound model: " + inSoundModelName);
                e.printStackTrace();
                return null;
            }
        } else {
            Log.v(TAG, "getSoundModelFromName: sound model with name: " + inSoundModelName +
                    " does not exist");
            return null;
        }
    }

    // prints all SMs and all info in them for debugging purposes
    private String printSms() {
        StringBuilder smsToText = new StringBuilder();
        smsToText.append("\n");
        for (SoundModel soundModel : soundModels) {
            smsToText.append("\t" + soundModel.getName() + "\n");
            for (Keyphrase keyphrase : soundModel.getKeyphrases()) {
                smsToText.append("\t\t" + keyphrase.getName() + "\n");
                for (User user : keyphrase.getUsers()) {
                    smsToText.append("\t\t\t" + user.getName() + "\n");
                }
            }
        }
        return smsToText.toString();
    }

    /*** Sound Trigger Functions ***/
    public String findKeyphraseOrUserById(String soundModelName, int keyphraseOrUserId) {
        SoundModel soundModel = getSmByName(soundModelName);
        if (null == soundModel) {
            Log.e(TAG, "findKeyphraseOrUserById: Could not find soundModel with name= " +
                    soundModelName);
            return null;
        }

        for (Keyphrase keyphrase : soundModel.getKeyphrases())
        {
            if (keyphraseOrUserId == keyphrase.id) {
                return keyphrase.name;
            }
            for (User user : keyphrase.getUsers()) {
                if (keyphraseOrUserId == user.id) {
                    return user.name;
                }
            }
        }
        Log.e(TAG, "findKeyphraseOrUserById: Could not find keyphrase or user with id= " +
                keyphraseOrUserId);
        return null;
    }


















    //TODO eliminate what isn't needed
    // from Global


    //TODO implement this
    /*// Returns the keyword only sound model associated with the currently selected sound model
    public ByteBuffer getKeywordOnlySoundModel(String inKeyword) {
        Log.v(TAG, "getKeywordOnlySoundModel");
        if (null == inKeyword) {
            Log.v(TAG, "getKeywordOnlySoundModel: keywordOnlySoundModel already set");
            if (null == this.keywordOnlySoundModel) {
                Log.v(TAG, "getKeywordOnlySoundModel: keywordOnlySoundModel is null");
            } else {
                Log.v(TAG, "getKeywordOnlySoundModel: keywordOnlySoundModel is not null");
            }
            return this.keywordOnlySoundModel;
        }

        if (null == this.keywordOnlySoundModel) {
            Log.v(TAG, "getKeywordOnlySoundModel: generating with inKeyword= " + inKeyword);
            String filePath = generateSoundModelFilePath(inKeyword, false);
            try {
                this.keywordOnlySoundModel = Utils.readByteBufferFromFile(filePath);
            } catch (IOException e) {
                Log.e(TAG, "getKeywordOnlySoundModel: failed to read in user-independent SM");
                e.printStackTrace();
            }
        }

        return this.keywordOnlySoundModel;
    }

    //TODO implement this in SMRepo
    public void addSoundModel(SoundModel soundModelToAdd) {
        soundModels.add(soundModelToAdd);
    }

    //TODO implement this in SMRepo
    public void saveSoundModelChanges(SoundModel soundModel) {
        Log.v(TAG, "saveSoundModelChanges- soundModels.size() before save: " + soundModels.size());
        soundModels.set(queriedSoundModelIndex, soundModel);


        //TODO remove after testing
        soundModel = getSoundModels().get(getQueriedSoundModelIndex());
        for (Keyword keyword : soundModel.getKeywords()) {
            Log.v(TAG, "initializeUserInterface- keyword: " + keyword.getName());
            Log.v(TAG, "initializeUserInterface- keyword.getLaunch(): " + keyword.getLaunch());
        }

        Log.v(TAG, "saveSoundModelChanges- soundModels.size() after save: " + soundModels.size());
        Log.v(TAG, "saveSoundModelChanges- changes saved");
    }

    //TODO convert to use one sound model arraylist and remove this function (possibly)
    public ArrayList<SoundModel> getSoundModels() {
        return this.soundModels;
    }

    //TODO implement this in SMRepo
    public void setQueriedSoundModelIndex(String inSoundModelName) {
        for (int i = 0; i < soundModels.size(); i++) {
            Log.v(TAG, "setQueriedSoundModelIndex- soundModels.get(i).getName(): " +
                    soundModels.get(i).getName());
            if (soundModels.get(i).getName().equals(inSoundModelName)) {
                Log.v(TAG, "setQueriedSoundModelIndex- index: " + i);
                this.queriedSoundModelIndex = i;
                break;
            }
        }
    }

    public int getQueriedSoundModelIndex() {
        Log.v(TAG, "getQueriedSoundModelIndex- index: " + this.queriedSoundModelIndex);
        return this.queriedSoundModelIndex;
    }*/
}
