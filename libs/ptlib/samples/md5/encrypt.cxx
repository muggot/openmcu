/*
 * md5.cxx
 *
 * Copyright 2005 Indranet Technologies Ltd
 *
 * Author: Derek J Smithies, <derek@indranet.co.nz>
 *
 * $Log: encrypt.cxx,v $
 * Revision 1.3  2006/06/21 03:28:42  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.2  2005/11/30 12:47:40  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.1  2005/03/02 21:50:03  dereksmithies
 * Initial release of example code to calculate MD5 sums of a supplied string.
 *
 *
 *
 */
#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptclib/cypher.h>


#define new PNEW

class Md5 : public PProcess
{
  PCLASSINFO(Md5, PProcess);
public:
  Md5();
  void Main();
};

PCREATE_PROCESS(Md5);

Md5::Md5()
  : PProcess("PwLib Example Factory", "md5", 1, 0, ReleaseCode, 0)
{

}

void Md5::Main()
{

  PConfigArgs args(GetArguments());
  args.Parse(
       "a-first: "
       "b-second: "
       "h-help. "
#if PTRACING
       "o-output:"             "-no-output."
       "t-trace: "             "-no-trace."
#endif
       , FALSE);

  cout << "Product Name: " << GetName() << endl
       << "Manufacturer: " << GetManufacturer() << endl
       << "Version     : " << GetVersion(TRUE) << endl
       << "System      : " << GetOSName() << '-'
                           << GetOSHardware() << ' '
                           << GetOSVersion() << endl
       << endl;


  if (args.HasOption('h')) {
    cout << "Usage :  md5 [options] " << endl 
         << endl
         << " -a  --first  ## : specify first string to add to md5" << endl
         << " -b  --second ## : specify second string to add to md5" << endl
         << " -h  --help      : print this help out." << endl

#if PTRACING
         << " -t --trace     : Enable trace, use multiple times for more detail"  << endl 
         << " -o --output    : File for trace output, default is stderr"  << endl
#endif
         << endl;
    return;
  }

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
                     PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif


  PString a = args.GetOptionString('a', "127000151");
  PString b = args.GetOptionString('b', "ebey7" );

  PMessageDigest5 stomach;
  stomach.Process(a);
  stomach.Process(b);
  PMessageDigest5::Code digester;
  stomach.Complete(digester);

  cerr << "Resultant MD5 output is " << endl 
       << PBase64::Encode(&digester, sizeof(digester)) << endl << endl;

  for(PINDEX i = 0; i < (PINDEX)sizeof(digester); i++) 
    cerr << ::hex << ::setfill('0') << ::setw(2) << (int)(*(((BYTE *)&digester)+i));

  cerr << ::dec << endl;

#if PTRACING
  if (args.GetOptionCount('t') > 0) {
    PTrace::ClearOptions(0);
    PTrace::SetLevel(0);
  }
#endif

}
// End of encrypt.cxx

/***

/Two examples of operation.

a= 142678697
b = h2f8v

gives e6e718481e199e77649c88e0b48c40eb


a= 127000151
b= ebey7

gives 07065e2a12d38ca389d8c97ad4291427


***/
