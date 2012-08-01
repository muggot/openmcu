/*
 * asnper.h
 *
 * Abstract Syntax Notation Encoding Rules classes
 *
 * Portable Windows Library
 *
 */

#ifdef P_INCLUDE_PER

/** Class for ASN Packed Encoding Rules stream.
*/
class PPER_Stream : public PASN_Stream
{
    PCLASSINFO(PPER_Stream, PASN_Stream);
  public:
    PPER_Stream(BOOL aligned = TRUE);
    PPER_Stream(const PBYTEArray & bytes, BOOL aligned = TRUE);
    PPER_Stream(const BYTE * buf, PINDEX size, BOOL aligned = TRUE);

    PPER_Stream & operator=(const PBYTEArray & bytes);

    unsigned GetBitsLeft() const;

    virtual BOOL Read(PChannel & chan);
    virtual BOOL Write(PChannel & chan);

    virtual BOOL NullDecode(PASN_Null &);
    virtual void NullEncode(const PASN_Null &);
    virtual BOOL BooleanDecode(PASN_Boolean &);
    virtual void BooleanEncode(const PASN_Boolean &);
    virtual BOOL IntegerDecode(PASN_Integer &);
    virtual void IntegerEncode(const PASN_Integer &);
    virtual BOOL EnumerationDecode(PASN_Enumeration &);
    virtual void EnumerationEncode(const PASN_Enumeration &);
    virtual BOOL RealDecode(PASN_Real &);
    virtual void RealEncode(const PASN_Real &);
    virtual BOOL ObjectIdDecode(PASN_ObjectId &);
    virtual void ObjectIdEncode(const PASN_ObjectId &);
    virtual BOOL BitStringDecode(PASN_BitString &);
    virtual void BitStringEncode(const PASN_BitString &);
    virtual BOOL OctetStringDecode(PASN_OctetString &);
    virtual void OctetStringEncode(const PASN_OctetString &);
    virtual BOOL ConstrainedStringDecode(PASN_ConstrainedString &);
    virtual void ConstrainedStringEncode(const PASN_ConstrainedString &);
    virtual BOOL BMPStringDecode(PASN_BMPString &);
    virtual void BMPStringEncode(const PASN_BMPString &);
    virtual BOOL ChoiceDecode(PASN_Choice &);
    virtual void ChoiceEncode(const PASN_Choice &);
    virtual BOOL ArrayDecode(PASN_Array &);
    virtual void ArrayEncode(const PASN_Array &);
    virtual BOOL SequencePreambleDecode(PASN_Sequence &);
    virtual void SequencePreambleEncode(const PASN_Sequence &);
    virtual BOOL SequenceKnownDecode(PASN_Sequence &, PINDEX, PASN_Object &);
    virtual void SequenceKnownEncode(const PASN_Sequence &, PINDEX, const PASN_Object &);
    virtual BOOL SequenceUnknownDecode(PASN_Sequence &);
    virtual void SequenceUnknownEncode(const PASN_Sequence &);

    BOOL IsAligned() const { return aligned; }

    BOOL SingleBitDecode();
    void SingleBitEncode(BOOL value);

    BOOL MultiBitDecode(unsigned nBits, unsigned & value);
    void MultiBitEncode(unsigned value, unsigned nBits);

    BOOL SmallUnsignedDecode(unsigned & value);
    void SmallUnsignedEncode(unsigned value);

    BOOL LengthDecode(unsigned lower, unsigned upper, unsigned & len);
    void LengthEncode(unsigned len, unsigned lower, unsigned upper);

    BOOL UnsignedDecode(unsigned lower, unsigned upper, unsigned & value);
    void UnsignedEncode(int value, unsigned lower, unsigned upper);

    void AnyTypeEncode(const PASN_Object * value);

  protected:
    BOOL aligned;
};

#endif

