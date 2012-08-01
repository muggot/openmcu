/*
 * And more stuff stolen from Shawn Pai-Hsiang Hsiao
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *          
 */
 

/*
 * Returns the number of audio devices found.
 * The ID list is stored at *devlist
 */
static int CADeviceList(AudioDeviceID **devlist)
{
  OSStatus theStatus;
  UInt32 theSize;
  int numDevices;
  AudioDeviceID *deviceList;

  theStatus = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices,
					   &theSize, NULL);
  if (theStatus != 0) {
    (*devlist) = NULL;
    return 0;
  }

  numDevices = theSize/sizeof(AudioDeviceID);

  deviceList = (AudioDeviceID *)malloc(theSize);
  if (deviceList == NULL) {
    (*devlist) = NULL;
    return 0;
  }

  theStatus = AudioHardwareGetProperty(kAudioHardwarePropertyDevices,
				       &theSize, deviceList);
  if (theStatus != 0) {
    free(deviceList);
    (*devlist) = NULL;
    return 0;
  }

  (*devlist) = deviceList;
  return numDevices;
}


/*
 * Retrieves the name of the device, given its AudioDeviceID
 */
static PString CADeviceName(AudioDeviceID id)
{
  OSStatus theStatus;
  UInt32 theSize;
  char name[128];

  theSize = sizeof(name);
  theStatus = AudioDeviceGetProperty(id, 0, false,
				     kAudioDevicePropertyDeviceName,
				     &theSize, name);
  if (theStatus != 0 || *name == 0)
    return 0;

  return (PString(name));
}


/*
 * XXX: don't know what is the best way to decide if a device supports
 * input/output or not yet
 *
 * See if the device supports player/recorder direction.  This test is
 * based on how many streams the device has for that direction.
 */
static int CADeviceSupportDirection(AudioDeviceID id, bool isInput )
{
  OSStatus theStatus;
  UInt32 theSize;

  UInt32 numStreams;


  theStatus = AudioDeviceGetPropertyInfo(id, 0, isInput,
					 kAudioDevicePropertyStreams,
					 &theSize, NULL );

  if (theStatus == 0) {
    numStreams = theSize/sizeof(AudioStreamID);
    return (numStreams);
  }
  else {
    return (0);
  }
}


static AudioDeviceID GetDeviceID(PString deviceName, bool input){
  int numDevices;
  AudioDeviceID *deviceList;
  AudioDeviceID caDevID(kAudioDeviceUnknown);

  /*
   * Iterate through over devices names to find corresponding device ID
   */
  numDevices = CADeviceList(&deviceList);
  for (int i = 0; i < numDevices; i++) {
    PString s = CADeviceName(deviceList[i]);
    if ((CADeviceSupportDirection(deviceList[i], input) > 0) 
          && deviceName == s) 
    {
      caDevID = deviceList[i];
      break;
    }
  }

  if(deviceList != NULL){
	  free(deviceList);
	  deviceList = NULL;
  }

  PTRACE(1, "Searched id of " << deviceName << " and found " << caDevID);

  return caDevID;
}

