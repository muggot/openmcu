/*
 * main.cxx
 *
 * PWLib application source file for PxmlTest
 *
 * Main program entry point.
 *
 * Copyright 2002 David Iodice.
 *
 * $Log: main.cxx,v $
 * Revision 1.3  2006/07/22 07:27:26  rjongbloed
 * Fixed various compilation issues
 *
 * Revision 1.2  2005/11/30 12:47:40  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.1  2002/03/07 01:56:56  robertj
 * Added XML sample/test program.
 *
 */

#include <ptlib.h>
#include "main.h"


PCREATE_PROCESS(PxmlTest);



PxmlTest::PxmlTest()
  : PProcess("XML Testing Guru", "PxmlTest", 1, 0, AlphaCode, 1)
{
}

static const PString XML_ID("<?xml version=\"1.0\"?>");
static const PString LEThdr("<s:Envelope xmlns:s=\"http://schemas.letter.org/letter/envelope/\" s:encodingStyle=\"http://schemas.letter.org/letter/encoding/\"><s:Body>");
static const PString LETtrl("</s:Body></s:Envelope>");
static const PString SetNumHdr("<u:SetNum xmlns:u=\"urn:schemas-dustbowl:1\"><Num>");
static const PString SetNumTrl("</Num></u:SetNum>");

void printElement(int i,PXMLObject * el)
{
  if (el->IsElement())
  {
    cout << "("<<i++<<") "<< ((const PXMLElement *)el)->GetName() << endl;
    if (((const PXMLElement *)el)->HasAttributes())
    {
      for (PINDEX j=0; j< ((const PXMLElement *)el)->GetNumAttributes(); ++j)
      {
        cout << " " << ((const PXMLElement *)el)->GetKeyAttribute(j) << "=" << ((const PXMLElement *)el)->GetDataAttribute(j);
      }
    }
    cout << " " << ((const PXMLElement *)el)->GetData() << endl;
    if (((const PXMLElement *)el)->HasSubObjects())
    {
      PXMLObjectArray obs = ((const PXMLElement *)el)->GetSubObjects();
      for (PINDEX o=0;o<obs.GetSize();++o) 
        printElement(i,&obs[o]);
    }
  }
}

void PxmlTest::Main()
{
  PString t("Color");
  int num=5;
   PString COLORVAL("<Color>5</Color>");
   PString COLORT1("<Color>");
   PString COLORT2("</Color>");
   PString COLORT3("<Color/>");

  PString ns(PString::Signed,num);
  PAssert((COLORT1 == PXML::CreateStartTag(t)),"PXML::CreateStartTag(t) failed");
  PAssert((COLORT2 == PXML::CreateEndTag(t)),"PXML::CreateEndTag(t) failed");
  PAssert((COLORT3 == PXML::CreateTagNoData(t)),"PXML::CreateTagNoData(t) failed");
  PAssert((COLORVAL == PXML::CreateTag(t,ns)),"PXML::CreateTag(t,ns) failed");

  PString ch = LEThdr + SetNumHdr + "23" + SetNumTrl + LETtrl;

  PXML xml(ch,PXMLParser::Indent |PXMLParser::NewLineAfterElement+PXMLParser::NoIgnoreWhiteSpace);
  PStringStream s;
  s << xml;

  //can we recreate the XML?

  PXML revamp;
  PString ROOTELE("s:Envelope");
  PString ROOTATKEY1("xmlns:s");
  PString ROOTATDAT1("http://schemas.letter.org/letter/envelope/");
  PString ROOTATKEY2("s:encodingStyle");
  PString ROOTATDAT2("http://schemas.letter.org/letter/encoding/");
  PAssert((ROOTELE == xml.GetRootElement()->GetName()),"Root element name not as expected");
  if (xml.GetRootElement()->HasAttributes()) {
    for (PINDEX a=0; a<xml.GetRootElement()->GetNumAttributes(); ++a) {
      PAssert((ROOTATKEY1 == xml.GetRootElement()->GetKeyAttribute(a) || 
               ROOTATKEY2 == xml.GetRootElement()->GetKeyAttribute(a)),"Root element attribute key not as expected");
      PAssert((ROOTATDAT1 == xml.GetRootElement()->GetDataAttribute(a) || 
               ROOTATDAT2 == xml.GetRootElement()->GetDataAttribute(a)),"Root element attribute data not as expected");
    }
  }
  PAssert((PString() == xml.GetRootElement()->GetData()),"Root element data not as expected");

#if 0
  //===
  // An example of printing elements -- see the function defined above
  //===
  for (PINDEX i=0; i<xml.GetNumElements(); ++i)
  {
    printElement(i,xml.GetElement(i));
  }


  //===
  // Just checking reloading the same data in a different format
  //===
  PStringStream rs; xml.GetRootElement()->PrintOn(rs, -1, 0); 
  cout << rs << endl;
  revamp.Load(rs);;
  cout << "Revamped XML" << revamp << endl;
#endif

  //===
  // Extract a subset of the XML
  // This can be used when the "data" of an XML element 
  // is XML and you don't need that parsed yet :-)
  //===
  PStringStream ss;
  xml.GetElement(0)->Output(ss, -1, 0);

  PString EXCERPT("<s:Body>" + SetNumHdr + "23" + SetNumTrl + "</s:Body>");
  PAssert((EXCERPT == ss),"XML subset data not as expected");


  PXMLElement el(NULL, t);
  PXMLData * d = new PXMLData(NULL,ns);
  el.AddSubObject(d);
  PAssert((PString("Color") == el.GetName()),"Element name not as expected");
  PAssert((PString("5") == el.GetData()),"Element data not as expected");

#if 0
  //===
  // Checkout these examples of the PXMLSettings class
  //
  //  Constructor with data and options
  //===
  PXMLSettings dgi(ch,PXML::Indent | PXML::NewLineAfterElement | PXML::NoIgnoreWhiteSpace);
  cout << "PXMLSettings: " << dgi << endl;
  //  Default Constructor
  PXMLSettings opts;
  PString optsect("Options");
  PString comType("Comm Type");
  PString val("FTP");
  opts.SetAttribute(optsect,comType,val);
  cout << "attributes==>" << opts.GetAttribute(optsect,comType) << "<==" << endl;
  cout << "PXMLSettings: " << opts;
  cout << "done" << endl;
#endif

  //  Constructor with PConfig and String
  PFilePath fp("cfg.txt");
  PConfig cfg(fp, "Options");
  PXMLSettings xmlcfg(cfg);
  PStringStream xc; 
  xc << xmlcfg;
  PFilePath fp2("cfgOut.txt");
  PConfig * cfg2; cfg2 = new PConfig(fp2, "Root");
  xmlcfg.ToConfig(*cfg2);
  delete cfg2;
  PTextFile tfp(fp2);
  PString fxc;
  char xcc;
  while(tfp.Read((void *)&xcc,1)) 
    fxc += xcc;
  PXMLSettings xmlcfg2(cfg);
  PStringStream xc2; 
  xc2 << xmlcfg2;
  PAssert((xc2 == xc),"Config not as expected");

  cout << "*** Test Passed ***" << endl;
}

// End of File ///////////////////////////////////////////////////////////////
