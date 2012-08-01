/*
 * main.cxx - do wave file things.
 *
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.cxx,v $
 * Revision 1.5  2007/01/12 22:34:22  rjongbloed
 * Fixed compile error in tone sample app
 *
 * Revision 1.4  2006/07/22 07:27:26  rjongbloed
 * Fixed various compilation issues
 *
 * Revision 1.3  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.2  2005/08/18 00:24:50  dereksmithies
 * Tidyup so compiles on linux, tidy up copyright headers, add cvs logging.
 *
 *
 *
 */

#include <ptlib.h>
#include <ptclib/pwavfile.h>
#include <ptclib/dtmf.h>
#include <ptlib/sound.h>
#include <ptlib/pprocess.h>

#define SAMPLES 64000  

class WAVFileTest : public PProcess
{
  public:
    WAVFileTest()
    : PProcess() { }
    void Main();
};

PCREATE_PROCESS(WAVFileTest)

void WAVFileTest::Main()
{
  PArgList & args = GetArguments();
  args.Parse("p:  c: h. ");

  if (args.HasOption('h')) {
    cout << "usage: wavfile [-p device][-c fmt] fn" << endl;
    return;
  }

  if (args.HasOption('c')) {
    PString format = args.GetOptionString('c');
    PWAVFile file(format, args[0], PFile::WriteOnly);
    if (!file.IsOpen()) {
      cout << "error: cannot create file " << args[0] << endl;
      return;
    }

    //BYTE buffer[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    //file.Write(buffer, sizeof(buffer));

    PDTMFEncoder toneData;
    toneData.GenerateDialTone();
    PINDEX len = toneData.GetSize()*sizeof(short);
    file.Write((const short *)toneData, len);

    file.Close();
  }

  PWAVFile file(args[0], PFile::ReadOnly, PFile::MustExist, PWAVFile::fmt_NotKnown);
  if (!file.IsOpen()) {
    cout << "error: cannot open " << args[0] << endl;
    return;
  }

  PINDEX dataLen = file.GetDataLength();
  PINDEX hdrLen  = file.GetHeaderLength();
  PINDEX fileLen = file.GetLength();

  cout << "Format:       " << file.wavFmtChunk.format << " (" << file.GetFormatString() << ")" << "\n"
       << "Channels:     " << file.wavFmtChunk.numChannels << "\n"
       << "Sample rate:  " << file.wavFmtChunk.sampleRate << "\n"
       << "Bytes/sec:    " << file.wavFmtChunk.bytesPerSec << "\n"
       << "Bytes/sample: " << file.wavFmtChunk.bytesPerSample << "\n"
       << "Bits/sample:  " << file.wavFmtChunk.bitsPerSample << "\n"
       << "\n"
       << "Hdr length :  " << hdrLen << endl
       << "Data length:  " << dataLen << endl
       << "File length:  " << fileLen << " (" << hdrLen + dataLen << ")" << endl
       << endl;

  PBYTEArray data;
  if (!file.Read(data.GetPointer(dataLen), dataLen) || (file.GetLastReadCount() != dataLen)) {
    cout << "error: cannot read " << dataLen << " bytes of WAV data" << endl;
    return;
  }

  if (args.HasOption('p')) {

    PString service = args.GetOptionString('p');
    PString device;
    if (args.GetCount() > 0)
      device  = args[0];
    else if (service != "default") {
      PStringList deviceList = PSoundChannel::GetDeviceNames(service, PSoundChannel::Player);
      if (deviceList.GetSize() == 0) {
        cout << "error: No devices for sound service " << service << endl;
        return;
      }
      device = deviceList[0];
    }
    
    cout << "Using sound service " << service << " with device " << device << endl;

    PSoundChannel * snd;
    if (service == "default") {
      snd = new PSoundChannel();
      device = PSoundChannel::GetDefaultDevice(PSoundChannel::Player);
    }
    else {
      snd = PSoundChannel::CreateChannel(service);
      if (snd == NULL) {
        cout << "Failed to create sound service " << service << " with device " << device << endl;
        return;
      }
    }

    cout << "Opening sound service " << service << " with device " << device << endl;

    if (!snd->Open(device, PSoundChannel::Player)) {
      cout << "Failed to open sound service " << service << " with device " << device << endl;
      return;
    }

    if (!snd->IsOpen()) {
      cout << "Sound device " << device << " not open" << endl;
      return;
    }

    if (!snd->SetBuffers(SAMPLES, 2)) {
      cout << "Failed to set samples to " << SAMPLES << " and 2 buffers. End program now." << endl;
      return;
    }

    snd->SetVolume(50);

    if (!snd->Write((const BYTE *)data, data.GetSize())) {
      cout << "error: write to audio device failed" << endl;
      return;
    }

    snd->WaitForPlayCompletion();

  }
}
