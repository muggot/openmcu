/*
 * asnxer.cxx
 *
 * Abstract Syntax Notation 1 Encoding Rules
 *
 * Portable Windows Library
 *
 */

#include <ptclib/pxml.h>

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::NullDecode(PASN_Null &)
{
  return TRUE;
}


void PXER_Stream::NullEncode(const PASN_Null &)
{
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::BooleanDecode(PASN_Boolean & value)
{
  value = (position->GetElement("true") != 0);
  return TRUE;
}


void PXER_Stream::BooleanEncode(const PASN_Boolean & value)
{
  position->AddChild(new PXMLElement(position, value.GetValue() ? "true" : "false"));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::IntegerDecode(PASN_Integer & value)
{
  value = position->GetData().AsInteger();
  return TRUE;
}


void PXER_Stream::IntegerEncode(const PASN_Integer & value)
{
  position->AddChild(new PXMLData(position, value.GetValue()));
}

///////////////////////////////////////////////////////////////////////

BOOL PASN_Enumeration::DecodeXER(PXER_Stream & strm)
{
  value = strm.GetCurrentElement()->GetData().AsInteger();
  return TRUE;
}


void PASN_Enumeration::EncodeXER(PXER_Stream & strm) const
{
  PXMLElement * elem = strm.GetCurrentElement();
  elem->AddChild(new PXMLData(elem, value));
}


BOOL PXER_Stream::EnumerationDecode(PASN_Enumeration & value)
{
  return value.DecodeXER(*this);
}


void PXER_Stream::EnumerationEncode(const PASN_Enumeration & value)
{
  value.EncodeXER(*this);
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::RealDecode(PASN_Real & value)
{
  value = position->GetData().AsReal();
  return TRUE;
}


void PXER_Stream::RealEncode(const PASN_Real & value)
{
  position->AddChild(new PXMLData(position, PString(PString::Decimal, value.GetValue(), 10)));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::ObjectIdDecode(PASN_ObjectId & value)
{
  value.SetValue(position->GetData());
  return TRUE;
}


void PXER_Stream::ObjectIdEncode(const PASN_ObjectId & value)
{
  position->AddChild(new PXMLData(position, value.AsString()));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::BitStringDecode(PASN_BitString & value)
{
  PString bits = position->GetData();
  PINDEX len = bits.GetLength();

  value.SetSize(len);

  for (PINDEX i = 0 ; i < len ; i++)
  {
    if (bits[i] == '1')
      value.Set(i);
    else if (bits[i] != '0')
      return FALSE;
  }

  return TRUE;
}


void PXER_Stream::BitStringEncode(const PASN_BitString & value)
{
  PString bits;

  for (PINDEX i = 0 ; i < (PINDEX)value.GetSize() ; i++)
  {
    bits += (value[i] ? '1' : '0');
  }

  position->AddChild(new PXMLData(position, bits));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::OctetStringDecode(PASN_OctetString & value)
{
  char elem[3] = { 0, 0, 0 };
  PString data = position->GetData();
  PINDEX len = data.GetLength();

  if (len % 2)
    return FALSE;

  BYTE * bin = value.GetPointer(len / 2);
  unsigned octet;

  for (PINDEX i = 0, j = 0 ; i < len ; i += 2, j++)
  {
    elem[0] = data[i];
    elem[1] = data[i + 1];
    sscanf(elem, "%x", &octet);
    bin[j] = (BYTE)octet;
  }

  return TRUE;
}


void PXER_Stream::OctetStringEncode(const PASN_OctetString & value)
{
  PString bin;

  for (PINDEX i = 0 ; i < value.GetSize() ; i++)
  {
    unsigned v = (unsigned)value[i];
    bin.sprintf("%02x", v);
  }

  position->AddChild(new PXMLData(position, bin));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::ConstrainedStringDecode(PASN_ConstrainedString & value)
{
  value = position->GetData();
  return TRUE;
}


void PXER_Stream::ConstrainedStringEncode(const PASN_ConstrainedString & value)
{
  position->AddChild(new PXMLData(position, value.GetValue()));
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::BMPStringDecode(PASN_BMPString &)
{
  return FALSE;
}


void PXER_Stream::BMPStringEncode(const PASN_BMPString &)
{
}

///////////////////////////////////////////////////////////////////////

BOOL PASN_Choice::DecodeXER(PXER_Stream & strm)
{
  PXMLElement * elem = strm.GetCurrentElement();
  PXMLElement * choice_elem = (PXMLElement *)elem->GetElement();

  if (!choice_elem || !choice_elem->IsElement())
    return FALSE;

  for (unsigned int i = 0 ; i < namesCount ; i++)
  {
    if (choice_elem->GetName() == names[i].name)
    {
      tag = names[i].value;
      if (!CreateObject())
        return FALSE;
      strm.SetCurrentElement(choice_elem);
      BOOL res = choice->Decode(strm);
      strm.SetCurrentElement(elem);
      return res;
    }
  }

  return FALSE;
}


void PASN_Choice::EncodeXER(PXER_Stream & strm) const
{
  if (choice)
  {
    PXMLElement * elem = strm.GetCurrentElement();
    strm.SetCurrentElement(elem->AddChild(new PXMLElement(elem, GetTagName())));
    choice->Encode(strm);
    strm.SetCurrentElement(elem);
  }
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::ChoiceDecode(PASN_Choice & value)
{
  return value.DecodeXER(*this);
}


void PXER_Stream::ChoiceEncode(const PASN_Choice & value)
{
  value.EncodeXER(*this);
}

///////////////////////////////////////////////////////////////////////

BOOL PASN_Sequence::PreambleDecodeXER(PXER_Stream &)
{
  return TRUE;
}


void PASN_Sequence::PreambleEncodeXER(PXER_Stream &) const
{
}


BOOL PASN_Sequence::KnownExtensionDecodeXER(PXER_Stream &, PINDEX, PASN_Object &)
{
  return TRUE;
}


void PASN_Sequence::KnownExtensionEncodeXER(PXER_Stream &, PINDEX, const PASN_Object &) const
{
}


BOOL PASN_Sequence::UnknownExtensionsDecodeXER(PXER_Stream &)
{
  return TRUE;
}


void PASN_Sequence::UnknownExtensionsEncodeXER(PXER_Stream &) const
{
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::SequencePreambleDecode(PASN_Sequence & seq)
{
  return seq.PreambleDecodeXER(*this);
}


void PXER_Stream::SequencePreambleEncode(const PASN_Sequence & seq)
{
  seq.PreambleEncodeXER(*this);
}


BOOL PXER_Stream::SequenceKnownDecode(PASN_Sequence & seq, PINDEX fld, PASN_Object & field)
{
  return seq.KnownExtensionDecodeXER(*this, fld, field);
}


void PXER_Stream::SequenceKnownEncode(const PASN_Sequence & seq, PINDEX fld, const PASN_Object & field)
{
  seq.KnownExtensionEncodeXER(*this, fld, field);
}


BOOL PXER_Stream::SequenceUnknownDecode(PASN_Sequence & seq)
{
  return seq.UnknownExtensionsDecodeXER(*this);
}


void PXER_Stream::SequenceUnknownEncode(const PASN_Sequence & seq)
{
  seq.UnknownExtensionsEncodeXER(*this);
}

///////////////////////////////////////////////////////////////////////

BOOL PXER_Stream::ArrayDecode(PASN_Array & array)
{
  array.RemoveAll();

  unsigned size = position->GetSize();

  if (!array.SetSize(size))
    return FALSE;

  PXMLElement * elem = position;
  BOOL res = TRUE;

  for (PINDEX i = 0; i < (PINDEX)size; i++) {
    position = (PXMLElement *)elem->GetElement(i);

    if (!position->IsElement() || !array[i].Decode(*this)) {
      res = FALSE;
      break;
    }
  }

  position = elem;

  return res;
}


void PXER_Stream::ArrayEncode(const PASN_Array & array)
{
  PINDEX size = array.GetSize();
  PXMLElement * elem = position;

  for (PINDEX i = 0; i < (PINDEX)size; i++) {
    PString name = array[i].GetTypeAsString();
    name.Replace(" ", "_", TRUE);
    position = elem->AddChild(new PXMLElement(elem, name));
    array[i].Encode(*this);
  }

  position = elem;
}

///////////////////////////////////////////////////////////////////////

PXER_Stream::PXER_Stream(PXMLElement * elem)
  : position(PAssertNULL(elem))
{
}


PXER_Stream::PXER_Stream(PXMLElement * elem, const PBYTEArray & bytes)
  : PASN_Stream(bytes),
    position(PAssertNULL(elem))
{
}


PXER_Stream::PXER_Stream(PXMLElement * elem, const BYTE * buf, PINDEX size)
  : PASN_Stream(buf, size),
    position(PAssertNULL(elem))
{
}


BOOL PXER_Stream::Read(PChannel &)
{
  return FALSE;
}


BOOL PXER_Stream::Write(PChannel &)
{
  return FALSE;
}
