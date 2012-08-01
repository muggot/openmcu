/*
 * main.cxx
 *
 * PWLib application source file for XMLRPCApp
 *
 * Main program entry point.
 *
 * Copyright 2002 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.9  2007/09/17 11:14:45  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.8  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.7  2003/09/26 13:41:32  rjongbloed
 * Added special test to give more indicative error if try to compile without Expat support.
 *
 * Revision 1.6  2003/04/15 03:00:41  robertj
 * Added array support to XML/RPC
 * Fixed XML/RPC parsing when lots of white space in raw XML, caused by
 *   big fix to base XML parser not returning internal data elements.
 *
 * Revision 1.5  2002/12/04 02:09:17  robertj
 * Changed macro name prefix to PXMLRPC
 *
 * Revision 1.4  2002/12/04 00:16:18  robertj
 * Large enhancement to create automatically encoding and decoding structures
 *   using macros to build a class.
 *
 * Revision 1.3  2002/10/04 05:16:44  craigs
 * Changed for new XMLRPC code
 *
 * Revision 1.2  2002/03/27 01:54:40  craigs
 * Added ability to send random struct as request
 * Added ability to preview request without sending
 *
 * Revision 1.1  2002/03/26 07:05:28  craigs
 * Initial version
 *
 */

/*

  Example command lines

    http://time.xmlrpc.com/RPC2 currentTime.getCurrentTime 

    http://www.mirrorproject.com/xmlrpc mirror.Random

    http://xmlrpc.usefulinc.com/demo/server.php system.listMethods
    http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoString "A test!"
    -i http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoInteger 12
    -f http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoFloat 3.121
    -a http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoStringArray One Two Three Four
    -a -i http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoIntegerArray 11 222 3333 44444
    -a -f http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoIntegerArray 1.1 22.23 333.333 4444.4444
    -s http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoStruct first 1st second 2nd third 3rd
    -a -s http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoStructArray first 1st second 2nd third 3rd "," fourth 4th fifth 5th "," sixth 6th "," seventh 7th eigth 8th ninth 9th tenth 10th
    --echo-struct http://xmlrpc.usefulinc.com/demo/server.php interopEchoTests.echoStruct
    -s http://10.0.2.13:6666/RPC2 Function1 key value


 */

#include <ptlib.h>
#include "main.h"

#include <ptclib/pxmlrpc.h>

#if !P_EXPAT
#error Must have Expat XML support for this application
#endif



PXMLRPC_STRUCT_BEGIN(NestedStruct)
    PXMLRPC_STRING  (NestedStruct, PString, another_string);
    PXMLRPC_INTEGER (NestedStruct, int, another_integer);
PXMLRPC_STRUCT_END()

PXMLRPC_STRUCT_BEGIN     (TestStruct)
    PXMLRPC_STRING_INIT  (TestStruct, PString, a_string, "A string!");
    PXMLRPC_INTEGER_INIT (TestStruct, int, an_integer, 12);
    PXMLRPC_BOOLEAN_INIT (TestStruct, BOOL, a_boolean, TRUE);
    PXMLRPC_DOUBLE_INIT  (TestStruct, double, a_float, 3.14159);
    PXMLRPC_DATETIME     (TestStruct, PTime, a_date);
    PXMLRPC_BINARY       (TestStruct, PBYTEArray, a_binary);
    PXMLRPC_ARRAY_STRING (TestStruct, PStringArray, PCaselessString, a_string_array);
    PXMLRPC_ARRAY_INTEGER(TestStruct, int, an_integer_array);
    PXMLRPC_ARRAY_DOUBLE (TestStruct, float, a_float_array);
    PXMLRPC_STRUCT       (TestStruct, NestedStruct, nested_struct);
    PXMLRPC_ARRAY_STRUCT (TestStruct, NestedStruct, array_struct);
PXMLRPC_STRUCT_END()
 

PCREATE_PROCESS(XMLRPCApp);


/////////////////////////////////////////////////////////////////////////////

XMLRPCApp::XMLRPCApp()
  : PProcess("Equivalence", "XMLRPCApp", 1, 0, AlphaCode, 1)
{
}

void XMLRPCApp::Main()
{
  PINDEX i;
  PArgList & args = GetArguments();

  args.Parse("a-array."
             "d-debug."
             "f-float."
             "i-integer."
             "s-struct."
#if PTRACING
             "t-trace."
             "o-output:"
#endif
             "-echo-struct."
             );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL);
#endif

  if (args.GetCount() < 2) {
    PError << "usage: xmlrpc url method [parms...]" << endl;
    return;
  }

  PString url    = args[0];
  PString method = args[1];

  PXMLRPC rpc(url);

  PXMLRPCBlock request(method);
  PXMLRPCBlock response;

  if (args.HasOption("echo-struct")) {
    TestStruct ts;
    ts.a_date -= PTimeInterval(0, 0, 0, 0, 5);

    ts.a_binary.SetSize(10);
    for (i = 0; i < 10; i++)
      ts.a_binary[i] = (BYTE)(i+1);

    ts.a_string_array.SetSize(3);
    ts.a_string_array[0] = "first";
    ts.a_string_array[1] = "second";
    ts.a_string_array[2] = "third";

    ts.an_integer_array.SetSize(7);
    for (i = 0; i < ts.an_integer_array.GetSize(); i++)
      ts.an_integer_array[i] = i+1;

    ts.a_float_array.SetSize(5);
    for (i = 0; i < ts.a_float_array.GetSize(); i++)
      ts.a_float_array[i] = (float)(1.0/(i+2));

    ts.nested_struct.another_string = "Another string!";
    ts.nested_struct.another_integer = 345;

    ts.array_struct.SetSize(2);
    ts.array_struct.SetAt(0, new NestedStruct);
    ts.array_struct[0].another_string = "Structure one";
    ts.array_struct[0].another_integer = 11111;
    ts.array_struct.SetAt(1, new NestedStruct);
    ts.array_struct[1].another_string = "Structure two";
    ts.array_struct[1].another_integer = 22222;
    request.AddParam(ts);
  }
  else {
    if (args.HasOption('a')) {
      if (args.HasOption('s')) {
        PArray<PStringToString> array;
        PStringToString dict;
        PString key;
        for (i = 2; i < args.GetCount(); i++) {
          if (args[i] == ",") {
            array.SetAt(array.GetSize(), new PStringToString(dict));
            dict = PStringToString();
            key = PString::Empty();
          }
          else if (key.IsEmpty())
            key = args[i];
          else {
            dict.SetAt(key, args[i]);
            key = PString::Empty();
          }
        }

        if (!dict.IsEmpty())
          array.SetAt(array.GetSize(), new PStringToString(dict));

        request.AddArray(array);
      }
      else if (args.HasOption('i'))
        request.AddArray(args.GetParameters(2), "int");
      else if (args.HasOption('f'))
        request.AddArray(args.GetParameters(2), "double");
      else
        request.AddArray(args.GetParameters(2));
    }
    else if (args.HasOption('s')) {
      PStringToString dict;
      for (i = 2; (i+1) < args.GetCount(); i += 2) {
        PString key   = args[i];
        PString value = args[i+1];
        dict.SetAt(key, value);
      }

      request.AddStruct(dict);
    }
    else {
      for (i = 2; i < args.GetCount(); i++) {
        if (args.HasOption('i'))
          request.AddParam((int)args[i].AsInteger());
        else if (args.HasOption('f'))
          request.AddParam(args[i].AsReal());
        else
          request.AddParam(args[i]);
      }
    }
  }

  if (args.HasOption('d'))
    cout << "Request = " << request << endl;

  if (!rpc.MakeRequest(request, response)) {
    PError << "Error in request (" 
           << rpc.GetFaultCode() 
           << ") : "
           << rpc.GetFaultText()
           << endl;
    return;
  }

  // scan through the response and print it out
  cout << "Response" << endl;
  for (i = 0; i < response.GetParamCount(); i++) {
    cout << "  " << i << ": ";
    PString type;
    PString val;
    if (response.GetParam(i, type, val)) {
      cout << type << " = ";
      if (type == "struct") {
        PStringToString dict;
        response.GetParam(i, dict);
        cout << '\n' << dict;
      }
      else if (type == "array") {
        PStringArray array;
        response.GetParam(i, array);
        cout << '\n' << setfill('\n') << array << setfill(' ');
      }
      else
        cout << val;
    }
    else
      cout << "error: " << response.GetFaultText();
    cout << endl;
  }

  if (args.HasOption("echo-struct")) {
    TestStruct ts;
    ts.a_date = PTime(0);
    if (response.GetParam(0, ts))
      cout << "Parsed response:\n" << ts;
    else
      cout << "Failed to parse resonse: " << response.GetFaultText();
    cout << endl;
  }
}

// End of File ///////////////////////////////////////////////////////////////
