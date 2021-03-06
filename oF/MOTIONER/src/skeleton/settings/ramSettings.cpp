//
//  ramSettings.cpp
//  MOTIONER
//
//  Created by Onishi Yoshito on 12/8/12.
//
//

#include "ramSettings.h"
#include "ramConstants.h"
#include "ofxException.h"
#include "ramSkeleton.h"
#include "ramUtils.h"
#include "ofxException.h"

#include "ofxEvent.h"
#include "ramEvent.h"

using namespace ram;
using namespace ram::skeleton;

//----------------------------------------------------------------------------------------
const int       Settings::kErrori = -1;
const float     Settings::kErrorf = -1234.5678;
const string    Settings::kErrors = "NoValue";


/// default values for this class and xml setting
//----------------------------------------------------------------------------------------
Settings::Settings() :
mIsLoaded(false),
mDeviceHostName(""),
mName(kErrors)
{
}

//----------------------------------------------------------------------------------------
Settings::~Settings()
{
}

//----------------------------------------------------------------------------------------
void Settings::load(const string &filePath)
{
    /// load a xml file
    mFileName = filePath;
    
    ofLogNotice("Settings") << "Loading \"" + filePath + "\"...";
    
    bool bLoaded = mXml.loadFile(filePath);
    if (!bLoaded)
        ofxThrowException(ofxException, "Couldn't load a xml file \""+filePath+"\"");
    
    mName = mXml.getValue("ACTOR_NAME", kErrors);
    _checkError("ACTOR_NAME", mName);

    /// notify loaded data
    ofxEventMessage m;
    m.setAddress(event::ADDRESS_CHANGE_SKELETON_NAME);
    m.addStringArg(mName);
    ofxNotifyEvent(m);
    
    ostringstream oss;
    //oss << "\"" << filePath << "\" loaded" << endl
    oss << "Name: " << mName;
    //<< "Device host name: \"" << mDeviceHostName /*<< "\" Device port: " << mXBeeWiFiPort*/ ;
    ofLogNotice("Settings") << oss.str();
    mIsLoaded = true;
}

//----------------------------------------------------------------------------------------
void Settings::save(const string &fileName)
{
    if (mIsLoaded) {
        string file = "";
        if (fileName.empty())
            file = mFileName;
        else
            file = fileName;
        
        mXml.saveFile(file);
        mFileName = file;
        bool bLoaded = mXml.loadFile(file);
        if (!bLoaded)
            ofxThrowException(ofxException, "Couldn't load a xml file \""+file+"\"");
    }
}

//----------------------------------------------------------------------------------------
void Settings::loadHierarchy(Skeleton *skeleton)
{    
    /// load hierarchy
    bool ret = mXml.pushTag("HIERARCHY");
    if (!ret)
        ofxThrowException(ofxException, "HIERARCHY tag didn't found!");
    
    ofLogNotice("Settings") << "Loading hierarchy settings...";
    string error;
    const string jointTag = "JOINT";
    const int nJoints = mXml.getNumTags(jointTag);
    if (nJoints != NUM_JOINTS)
        ofxThrowExceptionf(ofxException, "Number of joints is %d", nJoints);
    
    for (int i=0; i<NUM_JOINTS; i++) {
        /// set joint's name
        skeleton->mJoints.at(i).name = getJointName(i);
        skeleton->mJoints.at(i).id = i;
        const float x = mXml.getAttribute(jointTag, "PX", kErrorf, i);
        _checkError("PX", x);
        const float y = mXml.getAttribute(jointTag, "PY", kErrorf, i);
        _checkError("PY", y);
        const float z = mXml.getAttribute(jointTag, "PZ", kErrorf, i);
        _checkError("PZ", z);
        
        /// set joints local position
        skeleton->mJoints.at(i).setPosition(ofVec3f(x, y, z));
        //ofLogNotice() << skeleton->mJoints.at(i).name << ": "
        //<< skeleton->mJoints.at(i).getPosition();
    }
    
    mXml.popTag();
    
    //ofLogNotice("Settings") << "Loaded";
}

//----------------------------------------------------------------------------------------
void Settings::saveHierarchy(int nodeId, const ofVec3f &offset)
{
    bool ret = mXml.pushTag("HIERARCHY");
    if (!ret)
        ofxThrowException(ofxException, "HIERARCHY tag didn't found!");
    
    {
        int ret = 0;
        ret = mXml.setAttribute("JOINT", "PX", offset.x, nodeId);
        _checkOffertAtrribExist(ret);
        ret = mXml.setAttribute("JOINT", "PY", offset.y, nodeId);
        _checkOffertAtrribExist(ret);
        ret = mXml.setAttribute("JOINT", "PZ", offset.z, nodeId);
        _checkOffertAtrribExist(ret);
    }
    
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::loadCalibration(Skeleton *skeleton)
{
    ofLogNotice("Settings") << "Loading calibration settings...";
    
    bool ret = mXml.pushTag("CALIBRATION");
    if (!ret)
        ofxThrowException(ofxException, "CALIBRATION tag didn't found!");
    
    {
        string error;
        const string jointTag = "JOINT";
        const int nJoints = mXml.getNumTags(jointTag);
        if (nJoints != NUM_JOINTS)
            ofxThrowExceptionf(ofxException, "Number of joints is %d", nJoints);
        
        for (int i=0; i<NUM_JOINTS; i++) {
            const float x = mXml.getAttribute(jointTag, "RX", kErrorf, i);
            _checkError("RX", x);
            const float y = mXml.getAttribute(jointTag, "RY", kErrorf, i);
            _checkError("RY", y);
            const float z = mXml.getAttribute(jointTag, "RZ", kErrorf, i);
            _checkError("RZ", z);
            const float w = mXml.getAttribute(jointTag, "RW", kErrorf, i);
            _checkError("RW", w);
            
            /// set joints local position
            skeleton->mBaseFrame.rotation.at(i).set(x, y, z, w);
        }
        
        //ofLogNotice("Settings") << "Loaded";
    }
    
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::saveCalibration(Skeleton *skeleton)
{
    bool ret = mXml.pushTag("CALIBRATION");
    if (!ret)
        ofxThrowException(ofxException, "CALIBRATION tag didn't found!");
    
    {
        for (int i=0; i<NUM_JOINTS; i++) {
            const ofQuaternion q = skeleton->mBaseFrame.rotation.at(i);
            
            int ret = 0;
            const string jointTag = "JOINT";
            ret = mXml.setAttribute(jointTag, "RX", q.x(), i);
            _checkOffertAtrribExist(ret);
            ret = mXml.setAttribute(jointTag, "RY", q.y(), i);
            _checkOffertAtrribExist(ret);
            ret = mXml.setAttribute(jointTag, "RZ", q.z(), i);
            _checkOffertAtrribExist(ret);
            ret = mXml.setAttribute(jointTag, "RW", q.w(), i);
            _checkOffertAtrribExist(ret);
            
        }
    }
    
    mXml.popTag();
}

//----------------------------------------------------------------------------------------
void Settings::setName(const string &name)
{
    mName = name;
    mXml.setValue("ACTOR_NAME", mName);
}

/// xml utils
//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, int i)
{
    if (i==kErrori)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, float f)
{
    if (f==kErrorf)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkError(const string &name, const string &s)
{
    if (s == kErrors)
        ofxThrowException(ofxException, "Couldn't load a setting which is tagged "+name);
}

//----------------------------------------------------------------------------------------
void Settings::_checkOffertAtrribExist(int tagId)
{
    if (tagId < 0 || tagId >= mXml.getNumTags("JOINT"))
        ofxThrowException(ofxException, "No such tag or attribute found!");
}


